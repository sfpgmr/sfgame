#include "pch.h"
#include "RichTextColumns.h"

using namespace sfgame::Common;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Interop;

/// <summary>
/// <see cref="RichTextColumns"/> クラスの新しいインスタンスを初期化します。
/// </summary>
RichTextColumns::RichTextColumns()
{
	HorizontalAlignment = Windows::UI::Xaml::HorizontalAlignment::Left;
}

static DependencyProperty^ _columnTemplateProperty =
	DependencyProperty::Register("ColumnTemplate", TypeName(DataTemplate::typeid), TypeName(RichTextColumns::typeid),
	ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(
	&RichTextColumns::ResetOverflowLayout)));

/// <summary>
/// <see cref="ColumnTemplate"/> 依存関係プロパティを識別します。
/// </summary>
DependencyProperty^ RichTextColumns::ColumnTemplateProperty::get()
{
	return _columnTemplateProperty;
}

static DependencyProperty^ _richTextContentProperty =
	DependencyProperty::Register("RichTextContent", TypeName(RichTextBlock::typeid), TypeName(RichTextColumns::typeid),
	ref new PropertyMetadata(nullptr, ref new PropertyChangedCallback(
	&RichTextColumns::ResetOverflowLayout)));

/// <summary>
/// <see cref="RichTextContent"/> 依存関係プロパティを識別します。
/// </summary>
DependencyProperty^ RichTextColumns::RichTextContentProperty::get()
{
	return _richTextContentProperty;
}

/// <summary>
/// 列のレイアウトを再作成するため、コンテンツまたはオーバーフローのテンプレートを変更するときに呼び出されます。
/// </summary>
/// <param name="d">変更が発生した <see cref="RichTextColumns"/> の
/// インスタンス。</param>
/// <param name="e">特定の変更を説明するイベント データ。</param>
void RichTextColumns::ResetOverflowLayout(DependencyObject^ d, DependencyPropertyChangedEventArgs^ e)
{
	(void) e;	// 未使用のパラメーター

	auto target = dynamic_cast<RichTextColumns^>(d);
	if (target != nullptr)
	{
		// 大幅な変更が行われた場合は、最初からレイアウトをビルドし直します
		target->_overflowColumns = nullptr;
		target->Children->Clear();
		target->InvalidateMeasure();
	}
}

/// <summary>
/// 追加のオーバーフロー列が必要かどうか、および既存の列を削除できるかどうかを
/// 指定します。
/// </summary>
/// <param name="availableSize">空き領域のサイズは、作成できる追加の列の
/// 数の制限に使用されます。</param>
/// <returns>元のコンテンツと追加の列を合わせた最終的なサイズ。</returns>
Size RichTextColumns::MeasureOverride(Size availableSize)
{
	if (RichTextContent == nullptr)
	{
		Size emptySize(0, 0);
		return emptySize;
	}

	// RichTextBlock を子に指定するようにします。このとき、
	// 未完了であることを示すため、追加の列の一覧の不足箇所を
	// 使用します
	if (_overflowColumns == nullptr)
	{
		Children->Append(RichTextContent);
		_overflowColumns = ref new Vector<RichTextBlockOverflow^>();
	}

	// 最初は元の RichTextBlock コンテンツを基準にします
	RichTextContent->Measure(availableSize);
	auto maxWidth = RichTextContent->DesiredSize.Width;
	auto maxHeight = RichTextContent->DesiredSize.Height;
	auto hasOverflow = RichTextContent->HasOverflowContent;

	// オーバーフロー列を十分に確保します
	unsigned int overflowIndex = 0;
	while (hasOverflow && maxWidth < availableSize.Width && ColumnTemplate != nullptr)
	{
		// 既存のオーバーフロー列がなくなるまで使用した後、
		// 指定のテンプレートから作成します
		RichTextBlockOverflow^ overflow;
		if (_overflowColumns->Size > overflowIndex)
		{
			overflow = _overflowColumns->GetAt(overflowIndex);
		}
		else
		{
			overflow = safe_cast<RichTextBlockOverflow^>(ColumnTemplate->LoadContent());
			_overflowColumns->Append(overflow);
			Children->Append(overflow);
			if (overflowIndex == 0)
			{
				RichTextContent->OverflowContentTarget = overflow;
			}
			else
			{
				_overflowColumns->GetAt(overflowIndex - 1)->OverflowContentTarget = overflow;
			}
		}

		// 新しい列を基準にして、必要に応じて繰り返しの設定を行います
		Size remainingSize(availableSize.Width - maxWidth, availableSize.Height);
		overflow->Measure(remainingSize);
		maxWidth += overflow->DesiredSize.Width;
		maxHeight = __max(maxHeight, overflow->DesiredSize.Height);
		hasOverflow = overflow->HasOverflowContent;
		overflowIndex++;
	}

	// 不要な列をオーバーフロー チェーンから切断し、列のプライベート リストから削除して、
	// 子として削除します
	if (_overflowColumns->Size > overflowIndex)
	{
		if (overflowIndex == 0)
		{
			RichTextContent->OverflowContentTarget = nullptr;
		}
		else
		{
			_overflowColumns->GetAt(overflowIndex - 1)->OverflowContentTarget = nullptr;
		}
		while (_overflowColumns->Size > overflowIndex)
		{
			_overflowColumns->RemoveAt(overflowIndex);
			Children->RemoveAt(overflowIndex + 1);
		}
	}

	// 最終的に決定したサイズを報告します
	Size resultingSize(maxWidth, maxHeight);
	return resultingSize;
}

/// <summary>
/// 元のコンテンツと追加されたすべての列を整列します。
/// </summary>
/// <param name="finalSize">中で子を整列する必要がある領域のサイズを
/// 定義します。</param>
/// <returns>子が実際に必要とする領域のサイズ。</returns>
Size RichTextColumns::ArrangeOverride(Size finalSize)
{
	float maxWidth = 0;
	float maxHeight = 0;
	auto childrenIterator = Children->First();
	while (childrenIterator->HasCurrent)
	{
		auto child = childrenIterator->Current;
		Rect childRect(maxWidth, 0, child->DesiredSize.Width, finalSize.Height);
		child->Arrange(childRect);
		maxWidth += child->DesiredSize.Width;
		maxHeight = __max(maxHeight, child->DesiredSize.Height);
		childrenIterator->MoveNext();
	}
	Size resultingSize(maxWidth, maxHeight);
	return resultingSize;
}
