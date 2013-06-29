//
// MenuPage.xaml.h
// MenuPage クラスの宣言
//

#pragma once

#include "Common\LayoutAwarePage.h" // 生成されたヘッダーによって要求されます
#include "DataModel\MainMenuDataSource.h"
#include "MenuPage.g.h"
#include "App.xaml.h"

namespace sf
{
	/// <summary>
	/// アイテムのコレクションのプレビューを表示するページです。このページは、分割アプリケーションで使用できる
	/// グループを表示し、その 1 つを選択するために使用されます。
	/// </summary>
	public ref class MenuPage sealed
	{
	public:
		MenuPage();

	protected:
		virtual void LoadState(Platform::Object^ navigationParameter,
			Windows::Foundation::Collections::IMap<Platform::String^, Platform::Object^>^ pageState) override;
    virtual void SaveState(Windows::Foundation::Collections::IMap<Platform::String^, Platform::Object^>^ pageState) override {};
  private:
//    sf::App^ m_gameMain;
    void itemGridView_ItemClick(Platform::Object^ sender, Windows::UI::Xaml::Controls::ItemClickEventArgs^ e);
  };
}
