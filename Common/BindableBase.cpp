#include "pch.h"
#include "BindableBase.h"

using namespace sf::Common;

using namespace Platform;
using namespace Windows::UI::Xaml::Data;

/// <summary>
/// プロパティ値が変更されたことをリスナーに通知します。
/// </summary>
/// <param name="propertyName">リスナーに通知するために使用するプロパティの名前。</param>
void BindableBase::OnPropertyChanged(String^ propertyName)
{
	PropertyChanged(this, ref new PropertyChangedEventArgs(propertyName));
}

Windows::UI::Xaml::Data::ICustomProperty^ BindableBase::GetCustomProperty(Platform::String^ name) 
{
	return nullptr;
}

Windows::UI::Xaml::Data::ICustomProperty^ BindableBase::GetIndexedProperty(Platform::String^ name, Windows::UI::Xaml::Interop::TypeName type) 
{
	return nullptr;
}

Platform::String^ BindableBase::GetStringRepresentation() 
{
	return this->ToString(); 
}