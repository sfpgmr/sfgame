//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

#include "Common\LayoutAwarePage.h" // 生成されたヘッダーによって要求されます
#include "MainPage.g.h"
#include "ProductItem.h"

namespace sf
{
    ref class GameMain;

    public enum class GameInfoOverlayCommand
    {
        None,
        TapToContinue,
        PleaseWait,
        PlayAgain,
    };

    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class MainPage sealed
    {
    public:
        MainPage();

        void LicenseChanged(
            Windows::ApplicationModel::Store::ListingInformation^ listing,
            Windows::ApplicationModel::Store::LicenseInformation^ license
            );

        void SetProductItems(
            Windows::ApplicationModel::Store::ListingInformation^ listing,
            Windows::ApplicationModel::Store::LicenseInformation^ license
            );
        void OnWindowSizeChanged();

        Windows::UI::Xaml::Controls::SwapChainBackgroundPanel^ GetSwapChainBackgroundPanel() { return DXSwapChainPanel; };

        property Windows::UI::Xaml::Controls::Frame^ MainFrame 
        {
          Windows::UI::Xaml::Controls::Frame^ get() {return m_MainFrame;}
        }
    internal:
        void SetApp(GameMain^ app);

    protected:
        void OnPlayButtonClicked(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ args);
        void OnResetButtonClicked(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ args);
        void OnBuyAppButtonTapped(Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ args);
        void OnBuySelectorClicked(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ args);
        void OnChangeBackgroundButtonClicked(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ args);
        void OnResetLicenseButtonClicked(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ args);
        void OnGameInfoOverlayTapped(Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ args);
        void OnAppBarOpened(Object^ sender, Object^ args);
        void OnStoreReturnClicked(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ args);
        void OnLoadStoreClicked(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ args);

        void OptionalTrialUpgrade();
        void ShowStoreFlyout();
    private:

        GameMain^ m_gameMain;
        Windows::ApplicationModel::Store::LicenseInformation^ m_licenseInformation;
        Windows::ApplicationModel::Store::ListingInformation^ m_listingInformation;
        bool m_possiblePurchaseUpgrade;
        void GameAppBar_Closed(Platform::Object^ sender, Platform::Object^ e);
        void KillPlayer_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void ReturnMenu_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void m_PlayerKill_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
    };
}
