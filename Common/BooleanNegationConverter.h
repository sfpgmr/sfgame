#pragma once

namespace sfgame
{
	namespace Common
	{
		/// <summary>
		/// true を false に、および false を true に変換する値コンバーター。
		/// </summary>
		[Windows::Foundation::Metadata::WebHostHidden]
		public ref class BooleanNegationConverter sealed : Windows::UI::Xaml::Data::IValueConverter
		{
		public:
			virtual Platform::Object^ Convert(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object^ parameter, Platform::String^ language);
			virtual Platform::Object^ ConvertBack(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType, Platform::Object^ parameter, Platform::String^ language);
		};
	}
}
