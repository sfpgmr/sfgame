//
// SuspensionManager.h
// SuspensionManager クラスの宣言
//

#pragma once

#include <ppltasks.h>

namespace sfgame
{
	namespace Common
	{
		/// <summary>
		/// SuspensionManager は、グローバル セッション状態をキャプチャし、アプリケーションのプロセス継続時間管理を簡略化します。
		/// セッション状態は、さまざまな条件下で自動的にクリアされます。
		/// また、セッション間で伝達しやすく、アプリケーションのクラッシュや
		/// アップグレード時には破棄が必要な情報を格納する場合にのみ
		/// アップグレードされます。
		/// </summary>
		ref class SuspensionManager sealed
		{
		internal:
			static void RegisterFrame(Windows::UI::Xaml::Controls::Frame^ frame, Platform::String^ sessionStateKey);
			static void UnregisterFrame(Windows::UI::Xaml::Controls::Frame^ frame);
			static Concurrency::task<void> SaveAsync(void);
			static Concurrency::task<void> RestoreAsync(void);
			static property Windows::Foundation::Collections::IMap<Platform::String^, Platform::Object^>^ SessionState
			{
				Windows::Foundation::Collections::IMap<Platform::String^, Platform::Object^>^ get(void);
			};
			static Windows::Foundation::Collections::IMap<Platform::String^, Platform::Object^>^ SessionStateForFrame(
				Windows::UI::Xaml::Controls::Frame^ frame);

		private:
			static void RestoreFrameNavigationState(Windows::UI::Xaml::Controls::Frame^ frame);
			static void SaveFrameNavigationState(Windows::UI::Xaml::Controls::Frame^ frame);
		};
	}
}
