#include "pch.h"
#include "LayoutAwarePage.h"
#include "SuspensionManager.h"

using namespace sf::Common;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::System;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Navigation;

/// <summary>
/// <see cref="LayoutAwarePage"/> クラスの新しいインスタンスを初期化します。
/// </summary>
LayoutAwarePage::LayoutAwarePage()
{
	if (Windows::ApplicationModel::DesignMode::DesignModeEnabled)
	{
		return;
	}

	// 空の既定のビュー モデルを作成します
	DefaultViewModel = ref new Map<String^, Object^>(std::less<String^>());

	// このページがビジュアル ツリーの一部である場合、次の 2 つの変更を行います:
	// 1) アプリケーションのビューステートをページの表示状態にマップする
	// 2) キーボードおよびマウスのナビゲーション要求を処理する
	Loaded += ref new RoutedEventHandler(this, &LayoutAwarePage::OnLoaded);

	// ページが表示されない場合、同じ変更を元に戻します
	Unloaded += ref new RoutedEventHandler(this, &LayoutAwarePage::OnUnloaded);
}

static DependencyProperty^ _defaultViewModelProperty =
	DependencyProperty::Register("DefaultViewModel",
	TypeName(IObservableMap<String^, Object^>::typeid), TypeName(LayoutAwarePage::typeid), nullptr);

/// <summary>
/// <see cref="DefaultViewModel"/> 依存関係プロパティを識別します。
/// </summary>
DependencyProperty^ LayoutAwarePage::DefaultViewModelProperty::get()
{
	return _defaultViewModelProperty;
}

/// <summary>
/// <see cref="IObservableMap&lt;String, Object&gt;"/> の実装を取得します。これは、
/// 単純なビュー モデルとして使用されるように設計されています。
/// </summary>
IObservableMap<String^, Object^>^ LayoutAwarePage::DefaultViewModel::get()
{
	return safe_cast<IObservableMap<String^, Object^>^>(GetValue(DefaultViewModelProperty));
}

/// <summary>
/// <see cref="IObservableMap&lt;String, Object&gt;"/> の実装を設定します。これは、
/// 単純なビュー モデルとして使用されるように設計されています。
/// </summary>
void LayoutAwarePage::DefaultViewModel::set(IObservableMap<String^, Object^>^ value)
{
	SetValue(DefaultViewModelProperty, value);
}

/// <summary>
/// ページがビジュアル ツリーの一部である場合に呼び出されます
/// </summary>
/// <param name="sender">イベントをトリガーしたインスタンス。</param>
/// <param name="e">イベントが発生する条件を説明するイベント データ。</param>
void LayoutAwarePage::OnLoaded(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	this->StartLayoutUpdates(sender, e);

	// キーボードおよびマウスのナビゲーションは、ウィンドウ全体を使用する場合のみ適用されます
	if (this->ActualHeight == Window::Current->Bounds.Height &&
		this->ActualWidth == Window::Current->Bounds.Width)
	{
		// ウィンドウで直接待機するため、フォーカスは不要です
		_acceleratorKeyEventToken = Window::Current->CoreWindow->Dispatcher->AcceleratorKeyActivated +=
			ref new TypedEventHandler<CoreDispatcher^, AcceleratorKeyEventArgs^>(this,
			&LayoutAwarePage::CoreDispatcher_AcceleratorKeyActivated);
		_pointerPressedEventToken = Window::Current->CoreWindow->PointerPressed +=
			ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this,
			&LayoutAwarePage::CoreWindow_PointerPressed);
		_navigationShortcutsRegistered = true;
	}
}

/// <summary>
/// ページがビジュアル ツリーから削除される場合に呼び出されます
/// </summary>
/// <param name="sender">イベントをトリガーしたインスタンス。</param>
/// <param name="e">イベントが発生する条件を説明するイベント データ。</param>
void LayoutAwarePage::OnUnloaded(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if (_navigationShortcutsRegistered)
	{
		Window::Current->CoreWindow->Dispatcher->AcceleratorKeyActivated -= _acceleratorKeyEventToken;
		Window::Current->CoreWindow->PointerPressed -= _pointerPressedEventToken;
		_navigationShortcutsRegistered = false;
	}
	StopLayoutUpdates(sender, e);
}

#pragma region Navigation support

/// <summary>
/// イベント ハンドラーとして呼び出され、ページの関連付けられた <see cref="Frame"/> で前に戻ります。
/// ナビゲーション スタックの上部に到達するまで戻ります。
/// </summary>
/// <param name="sender">イベントをトリガーしたインスタンス。</param>
/// <param name="e">イベントが発生する条件を説明するイベント データ。</param>
void LayoutAwarePage::GoHome(Object^ sender, RoutedEventArgs^ e)
{
	(void) sender;	// 未使用のパラメーター
	(void) e;	// 未使用のパラメーター

	// ナビゲーション フレームを使用して最上位のページに戻ります
	if (Frame != nullptr)
	{
		while (Frame->CanGoBack)
		{
			Frame->GoBack();
		}
	}
}

/// <summary>
/// イベント ハンドラーとして呼び出され、このページの <see cref="Frame"/> に関連付けられた
/// ナビゲーション スタックで前に戻ります。
/// </summary>
/// <param name="sender">イベントをトリガーしたインスタンス。</param>
/// <param name="e">イベントが発生する条件を説明するイベント データ。</param>
void LayoutAwarePage::GoBack(Object^ sender, RoutedEventArgs^ e)
{
	(void) sender;	// 未使用のパラメーター
	(void) e;	// 未使用のパラメーター

	// ナビゲーション フレームを使用して前のページに戻ります
	if (Frame != nullptr && Frame->CanGoBack)
	{
		Frame->GoBack();
	}
}

/// <summary>
/// イベント ハンドラーとして呼び出され、このページの <see cref="Frame"/> に関連付けられた
/// ナビゲーション スタックで前に戻ります。
/// </summary>
/// <param name="sender">イベントをトリガーしたインスタンス。</param>
/// <param name="e">イベントが発生する条件を説明するイベント データ。</param>
void LayoutAwarePage::GoForward(Object^ sender, RoutedEventArgs^ e)
{
	(void) sender;	// 未使用のパラメーター
	(void) e;	// 未使用のパラメーター

	// ナビゲーション フレームを使用して次のページに進みます
	if (Frame != nullptr && Frame->CanGoForward)
	{
		Frame->GoForward();
	}
}

/// <summary>
/// このページがアクティブで、ウィンドウ全体を使用する場合、Alt キーの組み合わせなどのシステム キーを含む、
/// キーボード操作で呼び出されます。ページがフォーカスされていないときでも、
/// ページ間のキーボード ナビゲーションの検出に使用されます。
/// </summary>
/// <param name="sender">イベントをトリガーしたインスタンス。</param>
/// <param name="args">イベントが発生する条件を説明するイベント データ。</param>
void LayoutAwarePage::CoreDispatcher_AcceleratorKeyActivated(CoreDispatcher^ sender,
															 AcceleratorKeyEventArgs^ args)
{
	auto virtualKey = args->VirtualKey;

	// 左方向キーや右方向キー、または専用に設定した前に戻るキーや次に進むキーを押した場合のみ、
	// 詳細を調査します
	if ((args->EventType == CoreAcceleratorKeyEventType::SystemKeyDown ||
		args->EventType == CoreAcceleratorKeyEventType::KeyDown) &&
		(virtualKey == VirtualKey::Left || virtualKey == VirtualKey::Right ||
		(int)virtualKey == 166 || (int)virtualKey == 167))
	{
		auto coreWindow = Window::Current->CoreWindow;
		auto downState = Windows::UI::Core::CoreVirtualKeyStates::Down;
		bool menuKey = (coreWindow->GetKeyState(VirtualKey::Menu) & downState) == downState;
		bool controlKey = (coreWindow->GetKeyState(VirtualKey::Control) & downState) == downState;
		bool shiftKey = (coreWindow->GetKeyState(VirtualKey::Shift) & downState) == downState;
		bool noModifiers = !menuKey && !controlKey && !shiftKey;
		bool onlyAlt = menuKey && !controlKey && !shiftKey;

		if (((int)virtualKey == 166 && noModifiers) ||
			(virtualKey == VirtualKey::Left && onlyAlt))
		{
			// 前に戻るキーまたは Alt キーを押しながら左方向キーを押すと前に戻ります
			args->Handled = true;
			GoBack(this, ref new RoutedEventArgs());
		}
		else if (((int)virtualKey == 167 && noModifiers) ||
			(virtualKey == VirtualKey::Right && onlyAlt))
		{
			// 次に進むキーまたは Alt キーを押しながら右方向キーを押すと次に進みます
			args->Handled = true;
			GoForward(this, ref new RoutedEventArgs());
		}
	}
}

/// <summary>
/// このページがアクティブで、ウィンドウ全体を使用する場合、マウスのクリック、タッチ スクリーンのタップなどの
/// 操作で呼び出されます。ページ間を移動するため、マウス ボタンのクリックによるブラウザー スタイルの
/// 次に進むおよび前に戻る操作の検出に使用されます。
/// </summary>
/// <param name="sender">イベントをトリガーしたインスタンス。</param>
/// <param name="args">イベントが発生する条件を説明するイベント データ。</param>
void LayoutAwarePage::CoreWindow_PointerPressed(CoreWindow^ sender, PointerEventArgs^ args)
{
	auto properties = args->CurrentPoint->Properties;

	// 左、右、および中央ボタンを使用したボタン操作を無視します
	if (properties->IsLeftButtonPressed || properties->IsRightButtonPressed ||
		properties->IsMiddleButtonPressed) return;

	// [戻る] または [進む] を押すと適切に移動します (両方同時には押しません)
	bool backPressed = properties->IsXButton1Pressed;
	bool forwardPressed = properties->IsXButton2Pressed;
	if (backPressed ^ forwardPressed)
	{
		args->Handled = true;
		if (backPressed) GoBack(this, ref new RoutedEventArgs());
		if (forwardPressed) GoForward(this, ref new RoutedEventArgs());
	}
}

#pragma endregion

#pragma region Visual state switching

/// <summary>
/// これは通常、ページ内の <see cref="Control"/> の
/// <see cref="Loaded"/> イベントでイベント ハンドラーとして呼び出され、送信元がアプリケーションの
/// ビューステートの変更に対応する表示状態管理の変更を受信開始する必要があることを示します。
/// </summary>
/// <param name="sender">ビューステートに対応する表示状態管理をサポートする 
/// <see cref="Control"/> のインスタンス。</param>
/// <param name="e">要求がどのように行われたかを説明するイベント データ。</param>
/// <remarks>現在のビューステートは、レイアウトの更新が要求されると、対応する表示状態を設定するために
/// すぐに使用されます。対応する <see cref="Unloaded"/> イベント ハンドラーを
/// <see cref="StopLayoutUpdates"/> に接続しておくことを強くお勧めします。
/// <see cref="LayoutAwarePage"/> のインスタンスは、Loaded および Unloaded イベントでこれらのハンドラーを自動的に
/// 呼び出します。</remarks>
/// <seealso cref="DetermineVisualState"/>
/// <seealso cref="InvalidateVisualState"/>
void LayoutAwarePage::StartLayoutUpdates(Object^ sender, RoutedEventArgs^ e)
{
	(void) e;	// 未使用のパラメーター

	auto control = safe_cast<Control^>(sender);
	if (_layoutAwareControls == nullptr)
	{
		// 更新の対象となるコントロールがある場合、ビューステートの変更の待機を開始します
		_layoutAwareControls = ref new Vector<Control^>();
		_windowSizeEventToken = Window::Current->SizeChanged += ref new WindowSizeChangedEventHandler(this, &LayoutAwarePage::WindowSizeChanged);

		// ページで子に対する通知を受けます。すべてのコントロールに対するレイアウトの更新が停止されるまで、ページを保護します。
		_this = this;
	}
	_layoutAwareControls->Append(control);

	// コントロールの最初の表示状態を設定します
	VisualStateManager::GoToState(control, DetermineVisualState(ApplicationView::Value), false);
}

void LayoutAwarePage::WindowSizeChanged(Object^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ e)
{
	(void) sender;	// 未使用のパラメーター
	(void) e;	// 未使用のパラメーター

	InvalidateVisualState();
}

/// <summary>
/// これは通常、ページ内の <see cref="Control"/> の
/// <see cref="Unloaded"/> イベントでイベント ハンドラーとして呼び出され、送信元がアプリケーションのビューステートの変更に対応する
/// 表示状態管理の変更を受信開始する必要があることを示します。
/// </summary>
/// <param name="sender">ビューステートに対応する表示状態管理をサポートする 
/// <see cref="Control"/> のインスタンス。</param>
/// <param name="e">要求がどのように行われたかを説明するイベント データ。</param>
/// <remarks>現在のビューステートは、レイアウトの更新が要求されると、対応する表示状態を設定するために
/// すぐに使用されます。</remarks>
/// <seealso cref="StartLayoutUpdates"/>
void LayoutAwarePage::StopLayoutUpdates(Object^ sender, RoutedEventArgs^ e)
{
	(void) e;	// 未使用のパラメーター

	auto control = safe_cast<Control^>(sender);
	unsigned int index;
	if (_layoutAwareControls != nullptr && _layoutAwareControls->IndexOf(control, &index))
	{
		_layoutAwareControls->RemoveAt(index);
		if (_layoutAwareControls->Size == 0)
		{
			// 更新の対象となるコントロールがない場合、ビューステートの変更の待機を停止します
			Window::Current->SizeChanged -= _windowSizeEventToken;
			_layoutAwareControls = nullptr;
			// 最後のコントロールでアンロード通知を受けました。
			_this = nullptr;
		}
	}
}

/// <summary>
/// <see cref="ApplicationViewState"/> 値を、ページ内の表示状態管理で使用できる文字列に
/// 変換します。既定の実装では列挙値の名前を使用します。サブクラスで
/// このメソッドをオーバーライドして、使用されているマップ スキームを制御する場合があります。
/// </summary>
/// <param name="viewState">表示状態が必要なビューステート。</param>
/// <returns><see cref="VisualStateManager"/> の実行に使用される表示状態の名前</returns>
/// <seealso cref="InvalidateVisualState"/>
String^ LayoutAwarePage::DetermineVisualState(ApplicationViewState viewState)
{
	switch (viewState)
	{
	case ApplicationViewState::Filled:
		return "Filled";
	case ApplicationViewState::Snapped:
		return "Snapped";
	case ApplicationViewState::FullScreenPortrait:
		return "FullScreenPortrait";
	case ApplicationViewState::FullScreenLandscape:
	default:
		return "FullScreenLandscape";
	}
}

/// <summary>
/// 適切な表示状態を使用した表示状態の変更を待機しているすべてのコントロールを更新し
/// ます。
/// </summary>
/// <remarks>
/// 通常、ビューステートが変更されていない場合でも、別の値が返される可能性がある事を知らせるために
/// <see cref="DetermineVisualState"/> のオーバーライドと合わせて使用されます。
/// </remarks>
void LayoutAwarePage::InvalidateVisualState()
{
	if (_layoutAwareControls != nullptr)
	{
		String^ visualState = DetermineVisualState(ApplicationView::Value);
		auto controlIterator = _layoutAwareControls->First();
		while (controlIterator->HasCurrent)
		{
			auto control = controlIterator->Current;
			VisualStateManager::GoToState(control, visualState, false);
			controlIterator->MoveNext();
		}
	}
}

#pragma endregion

#pragma region Process lifetime management

/// <summary>
/// このページがフレームに表示されるときに呼び出されます。
/// </summary>
/// <param name="e">このページにどのように到達したかを説明するイベント データ。Parameter
/// プロパティは、表示するグループを示します。</param>
void LayoutAwarePage::OnNavigatedTo(NavigationEventArgs^ e)
{
	// ナビゲーションを通じてキャッシュ ページに戻るときに、状態の読み込みが発生しないようにします
	if (_pageKey != nullptr) return;

	auto frameState = SuspensionManager::SessionStateForFrame(Frame);
	_pageKey = "Page-" + Frame->BackStackDepth;

	if (e->NavigationMode == NavigationMode::New)
	{
		// 新しいページをナビゲーション スタックに追加するとき、次に進むナビゲーションの
		// 既存の状態をクリアします
		auto nextPageKey = _pageKey;
		int nextPageIndex = Frame->BackStackDepth;
		while (frameState->HasKey(nextPageKey))
		{
			frameState->Remove(nextPageKey);
			nextPageIndex++;
			nextPageKey = "Page-" + nextPageIndex;
		}

		// ナビゲーション パラメーターを新しいページに渡します
		LoadState(e->Parameter, nullptr);
	}
	else
	{
		// ナビゲーション パラメーターおよび保存されたページの状態をページに渡します。
		// このとき、中断状態の読み込みや、キャッシュから破棄されたページの再作成と同じ対策を
		// 使用します
		LoadState(e->Parameter, safe_cast<IMap<String^, Object^>^>(frameState->Lookup(_pageKey)));
	}
}

/// <summary>
/// このページがフレームに表示されなくなるときに呼び出されます。
/// </summary>
/// <param name="e">このページにどのように到達したかを説明するイベント データ。Parameter 
/// プロパティは、表示するグループを示します。</param>
void LayoutAwarePage::OnNavigatedFrom(NavigationEventArgs^ e)
{
	auto frameState = SuspensionManager::SessionStateForFrame(Frame);
	auto pageState = ref new Map<String^, Object^>();
	SaveState(pageState);
	frameState->Insert(_pageKey, pageState);
}

/// <summary>
/// このページには、移動中に渡されるコンテンツを設定します。前のセッションからページを
/// 再作成する場合は、保存状態も指定されます。
/// </summary>
/// <param name="navigationParameter">このページが最初に要求されたときに
/// <see cref="Frame.Navigate(Type, Object)"/> に渡されたパラメーター値。
/// </param>
/// <param name="pageState">前のセッションでこのページによって保存された状態のマップ。
/// ディクショナリ。ページに初めてアクセスするとき、状態は null になります。</param>
void LayoutAwarePage::LoadState(Object^ navigationParameter, IMap<String^, Object^>^ pageState)
{
}

/// <summary>
/// アプリケーションが中断される場合、またはページがナビゲーション キャッシュから破棄される場合、
/// このページに関連付けられた状態を保存します。値は、
/// <see cref="SuspensionManager.SessionState"/> のシリアル化の要件に準拠する必要があります。
/// </summary>
/// <param name="pageState">シリアル化可能な状態を設定される空のマップ。</param>
void LayoutAwarePage::SaveState(IMap<String^, Object^>^ pageState)
{
}

#pragma endregion
