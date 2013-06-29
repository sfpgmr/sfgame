//
// MainMenuDataSource.h
// MainMenuDataSource、MainMenuDataGroup、MainMenuDataItem、および MainMenuDataCommon クラスの宣言
//

#pragma once

#include <collection.h>
#include "Common\BindableBase.h"

// このファイルで定義されるデータ モデルは、メンバーが追加、削除、または変更されるときの通知をサポートする、
// 厳密に型指定されたモデルの代表的な例として機能します。選択された
// プロパティ名は標準の項目テンプレートのデータ バインドと一致します。
//
// アプリケーションでは、このモデルを開始位置として使用してこのモデル上でビルドするか、完全に破棄して
// 必要に応じて置き換えることがあります。

namespace sf
{
	namespace Data
	{
		ref class MainMenuDataGroup; // MainMenuDataItem と MainMenuDataGroup 間の循環関係を解決します

		/// <summary>
		/// <see cref="MainMenuDataItem"/> および <see cref="MainMenuDataGroup"/> の基本クラスでは、
		/// 両方に共通するプロパティを定義します。
		/// </summary>
		[Windows::Foundation::Metadata::WebHostHidden]
		[Windows::UI::Xaml::Data::Bindable]
    public ref class MainMenuDataCommon : sf::Common::BindableBase
		{
    internal:
      typedef std::function<void ()> OnClicked;
      MainMenuDataCommon(Platform::String^ uniqueId, Platform::String^ title, Platform::String^ subtitle, Platform::String^ imagePath,
        Platform::String^ description,OnClicked& ev);
		public:
      void SetImage(Platform::String^ path);
			virtual Platform::String^ GetStringRepresentation() override;
			property Platform::String^ UniqueId { Platform::String^ get(); void set(Platform::String^ value); }
			property Platform::String^ Title { Platform::String^ get(); void set(Platform::String^ value); }
			property Platform::String^ Subtitle { Platform::String^ get(); void set(Platform::String^ value); }
			property Windows::UI::Xaml::Media::ImageSource^ Image { Windows::UI::Xaml::Media::ImageSource^ get(); void set(Windows::UI::Xaml::Media::ImageSource^ value); }
			property Platform::String^ Description { Platform::String^ get(); void set(Platform::String^ value); }
      void Clicked(){onClicked_();}
		private:
			Platform::String^ uniqueId_;
			Platform::String^ title_;
			Platform::String^ subtitle_;
			Windows::UI::Xaml::Media::ImageSource^ image_;
			Platform::String^ imagePath_;
			Platform::String^ description_;
      OnClicked onClicked_;
		};

		/// <summary>
		/// 汎用項目データ モデル。
		/// </summary>
		[Windows::UI::Xaml::Data::Bindable]
		public ref class MainMenuDataItem sealed : MainMenuDataCommon
		{
		internal:
			MainMenuDataItem(Platform::String^ uniqueId, Platform::String^ title, Platform::String^ subtitle, Platform::String^ imagePath,
				Platform::String^ description, Platform::String^ content, MainMenuDataGroup^ group,OnClicked& ev);
    public:
			property Platform::String^ Content { Platform::String^ get(); void set(Platform::String^ value); }
			property MainMenuDataGroup^ Group { MainMenuDataGroup^ get(); void set(MainMenuDataGroup^ value); }

		private:
			Platform::WeakReference _group; // 参照カウント サイクルを中断するために使用する弱参照
			Platform::String^ _content;
		};

		/// <summary>
		/// 汎用グループ データ モデル。
		/// </summary>
		[Windows::UI::Xaml::Data::Bindable]
		public ref class MainMenuDataGroup sealed : public MainMenuDataCommon
		{
		internal:
			MainMenuDataGroup(Platform::String^ uniqueId, Platform::String^ title, Platform::String^ subtitle, Platform::String^ imagePath,
				Platform::String^ description,OnClicked& ev);
    public:
			property Windows::Foundation::Collections::IObservableVector<MainMenuDataItem^>^ Items
			{
				Windows::Foundation::Collections::IObservableVector<MainMenuDataItem^>^ get();
			}
			property Windows::Foundation::Collections::IVector<MainMenuDataItem^>^ TopItems
			{
				Windows::Foundation::Collections::IVector<MainMenuDataItem^>^ get();
			}

		private:
			Platform::Collections::Vector<MainMenuDataItem^>^ _items;
			Platform::Collections::Vector<MainMenuDataItem^>^ _topitems;
			void ItemsCollectionChanged(Windows::Foundation::Collections::IObservableVector<MainMenuDataItem^>^ , Windows::Foundation::Collections::IVectorChangedEventArgs^ );
		};

		/// <summary>
		/// ハードコーディングされたコンテンツを使用して、グループおよびアイテムのコレクションを作成します。
		/// 
		/// MainMenuDataSource はライブ プロダクション データではなくプレースホルダー データを使用して初期化するので
		/// サンプル データは設計時と実行時の両方に用意されています。
		/// </summary>
		[Windows::UI::Xaml::Data::Bindable]
		public ref class MainMenuDataSource sealed
		{
		public:			
			MainMenuDataSource();
			property Windows::Foundation::Collections::IObservableVector<MainMenuDataGroup^>^ AllGroups
			{
				Windows::Foundation::Collections::IObservableVector<MainMenuDataGroup^>^ get();
			}
      static property MainMenuDataSource^ Instance {
        MainMenuDataSource^ get();
      };
			static Windows::Foundation::Collections::IIterable<MainMenuDataGroup^>^ GetGroups(Platform::String^ uniqueId);
			static MainMenuDataGroup^ GetGroup(Platform::String^ uniqueId);
			static MainMenuDataItem^ GetItem(Platform::String^ uniqueId);

		private: 
			static void Init();
			Platform::Collections::Vector<MainMenuDataGroup^>^ _allGroups;
      static MainMenuDataSource^ instance_;
		};
	}
}
