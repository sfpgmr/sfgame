#pragma once

#include <collection.h>

namespace sf
{
	namespace Common
	{
		/// <summary>
		/// <see cref="RichTextBlock"/> のラッパーは、使用可能なコンテンツに合わせて、
		/// 必要なオーバーフロー列を追加で作成します。
		/// </summary>
		/// <example>
		/// 以下では、400 ピクセル幅の列に 50 ピクセルの余白が指定されたコレクションを作成します。
		/// これには、データ バインドされた任意のコンテンツが含まれます:
		/// <code>
		/// <RichTextColumns>
		///     <RichTextColumns.ColumnTemplate>
		///         <DataTemplate>
		///             <RichTextBlockOverflow Width="400" Margin="50,0,0,0"/>
		///         </DataTemplate>
		///     </RichTextColumns.ColumnTemplate>
		///     
		///     <RichTextBlock Width="400">
		///         <Paragraph>
		///             <Run Text="{Binding Content}"/>
		///         </Paragraph>
		///     </RichTextBlock>
		/// </RichTextColumns>
		/// </code>
		/// </example>
		/// <remarks>通常、バインドされていない領域で必要なすべての列を作成できる、
		/// 水平方向のスクロール領域で使用されます。垂直方向のスクロール領域で使用する場合、
		/// 列を追加で作成することはできません。</remarks>
		[Windows::UI::Xaml::Markup::ContentProperty(Name = "RichTextContent")]
		[Windows::Foundation::Metadata::WebHostHidden]
		public ref class RichTextColumns sealed : Windows::UI::Xaml::Controls::Panel
		{
		public:
			RichTextColumns();
			static property Windows::UI::Xaml::DependencyProperty^ RichTextContentProperty
			{
				Windows::UI::Xaml::DependencyProperty^ get();
			};
			static property Windows::UI::Xaml::DependencyProperty^ ColumnTemplateProperty
			{
				Windows::UI::Xaml::DependencyProperty^ get();
			}
			property Windows::UI::Xaml::DataTemplate^ ColumnTemplate
			{
				Windows::UI::Xaml::DataTemplate^ get() { return safe_cast<Windows::UI::Xaml::DataTemplate^>(GetValue(ColumnTemplateProperty)); }
				void set(Windows::UI::Xaml::DataTemplate^ value) { SetValue(ColumnTemplateProperty, value); }
			};
			property Windows::UI::Xaml::Controls::RichTextBlock^ RichTextContent
			{
				Windows::UI::Xaml::Controls::RichTextBlock^ get() { return safe_cast<Windows::UI::Xaml::Controls::RichTextBlock^>(GetValue(RichTextContentProperty)); }
				void set(Windows::UI::Xaml::Controls::RichTextBlock^ value) { SetValue(RichTextContentProperty, value); }
			};

		protected:
			virtual Windows::Foundation::Size MeasureOverride(Windows::Foundation::Size availableSize) override;
			virtual Windows::Foundation::Size ArrangeOverride(Windows::Foundation::Size finalSize) override;

		internal:
			static void ResetOverflowLayout(Windows::UI::Xaml::DependencyObject^ d, Windows::UI::Xaml::DependencyPropertyChangedEventArgs^ e);

		private:
			Platform::Collections::Vector<Windows::UI::Xaml::Controls::RichTextBlockOverflow^>^ _overflowColumns;
		};
	}
}
