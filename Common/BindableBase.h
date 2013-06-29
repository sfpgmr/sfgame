﻿#pragma once

namespace sf
{
	namespace Common
	{
		/// <summary>
		/// モデルを簡略化するための <see cref="INotifyPropertyChanged"/> の実装。
		/// </summary>
		[Windows::Foundation::Metadata::WebHostHidden]
		public ref class BindableBase : Windows::UI::Xaml::DependencyObject, Windows::UI::Xaml::Data::INotifyPropertyChanged, Windows::UI::Xaml::Data::ICustomPropertyProvider
		{
		public:
			virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;

		public:
			// ICustomPropertyProvider
			virtual Windows::UI::Xaml::Data::ICustomProperty^ GetCustomProperty(Platform::String^ name);
			virtual Windows::UI::Xaml::Data::ICustomProperty^ GetIndexedProperty(Platform::String^ name, Windows::UI::Xaml::Interop::TypeName type);
			virtual Platform::String^ GetStringRepresentation();

			property Windows::UI::Xaml::Interop::TypeName Type
			{
				virtual Windows::UI::Xaml::Interop::TypeName get() { return this->GetType(); }
			}


		protected:
			virtual void OnPropertyChanged(Platform::String^ propertyName);
		};
	}
}
