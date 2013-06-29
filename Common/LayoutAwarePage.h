#pragma once

#include <collection.h>

namespace sf
{
	namespace Common
	{
		/// <summary>
		/// Page を一般的な方法で実装すると、重要かつ便利な機能をいくつか使用できます:
		/// <list type="bullet">
		/// <item>
		/// <description>アプリケーションのビューステートと表示状態のマップ</description>
		/// </item>
		/// <item>
		/// <description>GoBack、GoForward、および GoHome イベント ハンドラー</description>
		/// </item>
		/// <item>
		/// <description>ナビゲーション用のマウスおよびキーボードのショートカット</description>
		/// </item>
		/// <item>
		/// <description>ナビゲーションの状態管理およびプロセス継続時間管理</description>
		/// </item>
		/// <item>
		/// <description>既定のビュー モデル</description>
		/// </item>
		/// </list>
		/// </summary>
		[Windows::Foundation::Metadata::WebHostHidden]
		public ref class LayoutAwarePage : Windows::UI::Xaml::Controls::Page
		{
		internal:
			LayoutAwarePage();

		public:
			void StartLayoutUpdates(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
			void StopLayoutUpdates(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
			void InvalidateVisualState();
			static property Windows::UI::Xaml::DependencyProperty^ DefaultViewModelProperty
			{
				Windows::UI::Xaml::DependencyProperty^ get();
			};
			property Windows::Foundation::Collections::IObservableMap<Platform::String^, Platform::Object^>^ DefaultViewModel
			{
				Windows::Foundation::Collections::IObservableMap<Platform::String^, Platform::Object^>^ get();
				void set(Windows::Foundation::Collections::IObservableMap<Platform::String^, Platform::Object^>^ value);
			}

		protected:
			virtual void GoHome(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
			virtual void GoBack(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
			virtual void GoForward(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
			virtual Platform::String^ DetermineVisualState(Windows::UI::ViewManagement::ApplicationViewState viewState);
			virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
			virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
			virtual void LoadState(Platform::Object^ navigationParameter,
				Windows::Foundation::Collections::IMap<Platform::String^, Platform::Object^>^ pageState);
			virtual void SaveState(Windows::Foundation::Collections::IMap<Platform::String^, Platform::Object^>^ pageState);

		private:
			Platform::String^ _pageKey;
			bool _navigationShortcutsRegistered;
			Platform::Collections::Map<Platform::String^, Platform::Object^>^ _defaultViewModel;
			Windows::Foundation::EventRegistrationToken _windowSizeEventToken,
				_acceleratorKeyEventToken, _pointerPressedEventToken;
			Platform::Collections::Vector<Windows::UI::Xaml::Controls::Control^>^ _layoutAwareControls;
			void WindowSizeChanged(Platform::Object^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ e);
			void OnLoaded(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
			void OnUnloaded(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
			void CoreDispatcher_AcceleratorKeyActivated(Windows::UI::Core::CoreDispatcher^ sender,
				Windows::UI::Core::AcceleratorKeyEventArgs^ args);
			void CoreWindow_PointerPressed(Windows::UI::Core::CoreWindow^ sender,
				Windows::UI::Core::PointerEventArgs^ args);
			LayoutAwarePage^ _this; // self への強い参照が OnUnload でクリーンアップされました
		};
	}
}
