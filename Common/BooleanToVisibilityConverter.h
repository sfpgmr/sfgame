#pragma once

namespace sf
{
	namespace Common
	{
		/// <summary>
		/// true を <see cref="Visibility::Visible"/> に、および false を 
		/// <see cref="Visibility::Collapsed"/> に変換する値コンバーター。
		/// </summary>
		[Windows::Foundation::Metadata::WebHostHidden]
		public ref class BooleanToVisibilityConverter sealed : Windows::UI::Xaml::Data::IValueConverter
		{
		public:
			virtual Platform::Object^ Convert(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object^ parameter, Platform::String^ language);
			virtual Platform::Object^ ConvertBack(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object^ parameter, Platform::String^ language);
		};
	}
}
