//
// MenuPage.xaml.cpp
// MenuPage クラスの実装
//

#include "pch.h"
#include "MenuPage.xaml.h"

using namespace sfgame;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// アイテム ページのアイテム テンプレートについては、http://go.microsoft.com/fwlink/?LinkId=234233 を参照してください

MenuPage::MenuPage()
{
	InitializeComponent();
}

/// <summary>
/// このページには、移動中に渡されるコンテンツを設定します。前のセッションからページを
/// 再作成する場合は、保存状態も指定されます。
/// </summary>
/// <param name="navigationParameter">このページが最初に要求されたときに
/// <see cref="Frame::Navigate(Type, Object)"/> に渡されたパラメーター値。
/// </param>
/// <param name="pageState">前のセッションでこのページによって保存された状態のマップ。
/// ディクショナリ。ページに初めてアクセスするとき、状態は null になります。</param>
void MenuPage::LoadState(Object^ navigationParameter, IMap<String^, Object^>^ pageState)
{
	(void) navigationParameter;	// 未使用のパラメーター
	(void) pageState;	// 未使用のパラメーター

	// TODO: バインド可能なアイテムのコレクションを、DefaultViewModel->("Items", <value>) を使って設定します
}
