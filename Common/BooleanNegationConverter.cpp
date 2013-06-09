#include "pch.h"
#include "BooleanNegationConverter.h"

using namespace sfgame::Common;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml::Interop;

Object^ BooleanNegationConverter::Convert(Object^ value, TypeName targetType, Object^ parameter, String^ language)
{
	(void) targetType;	// 未使用のパラメーター
	(void) parameter;	// 未使用のパラメーター
	(void) language;	// 未使用のパラメーター

	auto boxedBool = dynamic_cast<Box<bool>^>(value);
	auto boolValue = (boxedBool != nullptr && boxedBool->Value);
	return !boolValue;
}

Object^ BooleanNegationConverter::ConvertBack(Object^ value, TypeName targetType, Object^ parameter, String^ language)
{
	(void) targetType;	// 未使用のパラメーター
	(void) parameter;	// 未使用のパラメーター
	(void) language;	// 未使用のパラメーター

	auto boxedBool = dynamic_cast<Box<bool>^>(value);
	auto boolValue = (boxedBool != nullptr && boxedBool->Value);
	return !boolValue;
}
