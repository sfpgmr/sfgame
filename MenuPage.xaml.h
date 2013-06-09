//
// MenuPage.xaml.h
// MenuPage クラスの宣言
//

#pragma once

#include "Common\LayoutAwarePage.h" // 生成されたヘッダーによって要求されます
#include "MenuPage.g.h"

namespace sfgame
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
	};
}
