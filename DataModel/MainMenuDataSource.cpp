//
// MainMenuDataSource.cpp
// MainMenuDataSource、MainMenuDataGroup、MainMenuDataItem、および MainMenuDataCommon クラスの実装
//

#include "pch.h"
#include "DataModel\MainMenuDataSource.h"

using namespace sf::Data;
using namespace sf;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::ApplicationModel::Resources::Core;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Imaging;

//
// MainMenuDataCommon
//

MainMenuDataCommon::MainMenuDataCommon(String^ uniqueId, String^ title, String^ subtitle, String^ imagePath, String^ description,OnClicked& ev)
{
  onClicked_ = ev;
  uniqueId_ = uniqueId;
  title_ = title;
  subtitle_ = subtitle;
  description_ = description;
  imagePath_ = imagePath;
  image_ = nullptr;
}

String^ MainMenuDataCommon::UniqueId::get()
{
  return uniqueId_;
}

void MainMenuDataCommon::UniqueId::set(String^ value)
{
  if (uniqueId_ != value)
  {
    uniqueId_ = value;
    OnPropertyChanged("UniqueId");
  }
}

String^ MainMenuDataCommon::Title::get()
{
  return title_;
}

void MainMenuDataCommon::Title::set(String^ value)
{
  if (title_ != value)
  {
    title_ = value;
    OnPropertyChanged("Title");
  }
}

String^ MainMenuDataCommon::Subtitle::get()
{
  return subtitle_;
}

void MainMenuDataCommon::Subtitle::set(String^ value)
{
  if (subtitle_ != value)
  {
    subtitle_ = value;
    OnPropertyChanged("Subtitle");
  }
}

String^ MainMenuDataCommon::Description::get()
{
  return description_;
}

void MainMenuDataCommon::Description::set(String^ value)
{
  if (description_ != value)
  {
    description_ = value;
    OnPropertyChanged("Description");
  }
}

ImageSource^ MainMenuDataCommon::Image::get()
{
  static Uri^ _baseUri = ref new Uri("ms-appx:///");

  if (image_ == nullptr && imagePath_ != nullptr)
  {
    image_ = ref new BitmapImage(_baseUri->CombineUri(imagePath_));
  }
  return image_;
}

void MainMenuDataCommon::Image::set(ImageSource^ value)
{
  if (image_ != value)
  {
    image_ = value;
    imagePath_ = nullptr;
    OnPropertyChanged("Image");
    PropertySet set;
  }
}

void MainMenuDataCommon::SetImage(String^ path)
{
  image_ = nullptr;
  imagePath_ = path;
  OnPropertyChanged("Image");
}

Platform::String^ MainMenuDataCommon::GetStringRepresentation()
{
  return title_;
}

//
// MainMenuDataItem
//

MainMenuDataItem::MainMenuDataItem(String^ uniqueId, String^ title, String^ subtitle, String^ imagePath, String^ description,
                                   String^ content, MainMenuDataGroup^ group,OnClicked& ev)
                                   : MainMenuDataCommon(uniqueId, title, subtitle, imagePath, description,ev)
{
  _content = content;
  _group = group;
}

String^ MainMenuDataItem::Content::get()
{
  return _content;
}

void MainMenuDataItem::Content::set(String^ value)
{
  if (_content != value)
  {
    _content = value;
    OnPropertyChanged("Content");
  }
}

MainMenuDataGroup^ MainMenuDataItem::Group::get()
{
  return _group.Resolve<MainMenuDataGroup>();
}

void MainMenuDataItem::Group::set(MainMenuDataGroup^ value)
{
  if (Group != value)
  {
    _group = value;
    OnPropertyChanged("Group");
  }
}

//
// MainMenuDataGroup
//

MainMenuDataGroup::MainMenuDataGroup(String^ uniqueId, String^ title, String^ subtitle, String^ imagePath, String^ description,OnClicked& ev)
  : MainMenuDataCommon(uniqueId, title, subtitle, imagePath, description,ev)
{
  _items = ref new Vector<MainMenuDataItem^>();
  _topitems = ref new Vector<MainMenuDataItem^>();
  Items->VectorChanged +=
    ref new VectorChangedEventHandler<MainMenuDataItem^>(this,&MainMenuDataGroup::ItemsCollectionChanged,CallbackContext::Same);
}

void MainMenuDataGroup::ItemsCollectionChanged(Windows::Foundation::Collections::IObservableVector<MainMenuDataItem^>^ sender, Windows::Foundation::Collections::IVectorChangedEventArgs^ args)
{
  // バインドする完全な項目コレクションのサブセットを GroupedItemsPage から指定します。
  // これには次の 2 つの理由があります。GridView はサイズの大きい項目コレクションを仮想化しません。また、
  // グループを通じてたくさんのアイテムを参照するときに、ユーザー エクスペリエンスを
  // 向上します。
  //
  // 1、2、3、4、または 6 個の列が表示される場合でも、入力済みのグリッド コラムが使用されることになるため、
  // 最大 12 個のアイテムが表示されます

  if(args->CollectionChange == CollectionChange::Reset)
  {
    TopItems->Clear();
    return;
  }

  if(args->Index >= 12)
  {
    return;
  }

  switch(args->CollectionChange)
  {
  case CollectionChange::ItemInserted:
    TopItems->InsertAt(args->Index, Items->GetAt(args->Index));
    if(TopItems->Size > 12)
    {
      TopItems->RemoveAt(12);
    }
    break;
  case CollectionChange::ItemChanged:
    TopItems->SetAt(args->Index,Items->GetAt(args->Index));
    break;
  case CollectionChange::ItemRemoved:
    TopItems->RemoveAt(args->Index);
    if(Items->Size >= 12)
    {
      TopItems->Append(Items->GetAt(11));
    }
    break;
  }
}

IObservableVector<MainMenuDataItem^>^ MainMenuDataGroup::Items::get()
{
  return _items;
}

IVector<MainMenuDataItem^>^ MainMenuDataGroup::TopItems::get()
{
  return _topitems;
}

//
// MainMenuDataSource
//

MainMenuDataSource::MainMenuDataSource()
{
//  std::function<void ()> null_func([]()->void{});
  _allGroups = ref new Vector<MainMenuDataGroup^>();
/*
  auto group1 = ref new MainMenuDataGroup(L"GamePlayGroup",
    L"ゲームプレイ",
    L"ゲームプレイ",
    L"",
    L"",null_func);

  group1->Items->Append(ref new MainMenuDataItem("GamePlayGroup00",
    L"ゲームプレイ",
    L"ゲームプレイ",
    L"",
    L"",
    L"",
    group1,null_func));

  _allGroups->Append(group1);

  auto group2 = ref new MainMenuDataGroup(L"GameEditGroup",
    L"ゲームエディット",
    L"ゲームエディット",
    L"",
    L"",null_func);
  group2->Items->Append(ref new MainMenuDataItem("GameEditGroup01",
    L"敵動作エディット",
    L"敵動作エディット",
    L"",
    L"",
    L"",
    group2,null_func));

  group2->Items->Append(ref new MainMenuDataItem("GameEditGroup00",
    L"配置エディット",
    L"配置エディット",
    L"",
    L"",
    L"",
    group2,null_func));

  group2->Items->Append(ref new MainMenuDataItem("GameEditGroup03",
    L"配置エディット",
    L"配置エディット",
    L"",
    L"",
    L"",
    group2,null_func));

  group2->Items->Append(ref new MainMenuDataItem("GameEditGroup04",
    L"配置エディット",
    L"配置エディット",
    L"",
    L"",
    L"",
    group2,null_func));

  _allGroups->Append(group2);

*/

}

MainMenuDataSource^ MainMenuDataSource::instance_ = nullptr;

MainMenuDataSource^ MainMenuDataSource::Instance::get()
{
  if(instance_ == nullptr)
  {
    Init();
  }
  return instance_;
}

IObservableVector<MainMenuDataGroup^>^ MainMenuDataSource::AllGroups::get()
{
  return _allGroups;
}


void MainMenuDataSource::Init()
{
  if (instance_ == nullptr)
  {
    instance_ = ref new MainMenuDataSource();
  }
}
IIterable<MainMenuDataGroup^>^ MainMenuDataSource::GetGroups(String^ uniqueId)
{
  Init();
  String^ AllGroupsId = "AllGroups";
  if (!AllGroupsId->Equals(uniqueId)) throw ref new InvalidArgumentException("Only 'AllGroups' is supported as a collection of groups");

  return instance_->AllGroups;
}

MainMenuDataGroup^ MainMenuDataSource::GetGroup(String^ uniqueId)
{
  Init();
  // サイズの小さいデータ セットでは単純な一方向の検索を実行できます
  for each (auto group in instance_->AllGroups)
  {
    if (group->UniqueId->Equals(uniqueId)) return group;
  }
  return nullptr;
}

MainMenuDataItem^ MainMenuDataSource::GetItem(String^ uniqueId)
{
  Init();
  // サイズの小さいデータ セットでは単純な一方向の検索を実行できます
  for each (auto group in instance_->AllGroups)
  {
    for each (auto item in group->Items)
    {
      if (item->UniqueId->Equals(uniqueId)) return item;
    }
  }
  return nullptr;
}
