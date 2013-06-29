//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "App.xaml.h"
#include "MainPage.xaml.h"
#include "gamemain.h"
#include "ProductItem.h"

using namespace sf;

using namespace Microsoft::WRL;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::ApplicationModel::Store;
using namespace concurrency;

//----------------------------------------------------------------------

MainPage::MainPage() :
    m_possiblePurchaseUpgrade(false)
{
    InitializeComponent();

#ifdef USE_STORE_SIMULATOR
    ResetLicense->Visibility = ::Visibility::Visible;
#endif
}

//----------------------------------------------------------------------

void MainPage::SetApp(GameMain^ app)
{
    m_gameMain = app;
}

//----------------------------------------------------------------------

//void MainPage::HideGameInfoOverlay()
//{
//    VisualStateManager::GoToState(this, "NormalState", true);
//
//    GameAppBar->IsOpen = false;
//
//}

//----------------------------------------------------------------------

//void MainPage::ShowGameInfoOverlay()
//{
//    VisualStateManager::GoToState(this, "GameInfoOverlayState", true);
//}

//----------------------------------------------------------------------

//void MainPage::SetAction(GameInfoOverlayCommand action)
//{
//    // Enable only one of the four possible commands at the bottom of the
//    // Game Info Overlay.
//}

//----------------------------------------------------------------------

//void MainPage::SetGameLoading()
//{
//}

//----------------------------------------------------------------------

//void MainPage::SetGameStats(
//    int maxLevel,
//    int hitCount,
//    int shotCount
//    )
//{
//    m_possiblePurchaseUpgrade = true;
//    OptionalTrialUpgrade();
//}

//----------------------------------------------------------------------

//void MainPage::SetGameOver(
//    bool win,
//    int maxLevel,
//    int hitCount,
//    int shotCount,
//    int highScore
//    )
//{
//}

//----------------------------------------------------------------------

//void MainPage::SetLevelStart(
//    int level,
//    Platform::String^ objective,
//    float timeLimit,
//    float bonusTime
//    )
//{
//}

//----------------------------------------------------------------------

//void MainPage::SetPause(int level, int hitCount, int shotCount, float timeRemaining)
//{
//}
//
////----------------------------------------------------------------------
//
//void MainPage::SetSnapped()
//{
//    //VisualStateManager::GoToState(this, "SnappedState", true);
//}
//
////----------------------------------------------------------------------
//
//void MainPage::HideSnapped()
//{
//    //VisualStateManager::GoToState(this, "UnsnappedState", true);
//}

//----------------------------------------------------------------------

void MainPage::OnGameInfoOverlayTapped(Object^ /* sender */, TappedRoutedEventArgs^ /* args */)
{
    //m_gameMain->PressComplete();
}

//----------------------------------------------------------------------

void MainPage::OnPlayButtonClicked(Object^ /* sender */, RoutedEventArgs^ /* args */)
{
    //m_gameMain->PressComplete();
}

//----------------------------------------------------------------------

void MainPage::OnResetButtonClicked(Object^ /* sender */, RoutedEventArgs^ /* args */)
{
    //m_gameMain->ResetGame();
    GameAppBar->IsOpen = false;
}

//----------------------------------------------------------------------

void MainPage::LicenseChanged(
    ListingInformation^ listing,
    LicenseInformation^ license
    )
{
    m_listingInformation = listing;
    m_licenseInformation = license;

    // This function may be called from a different thread.
    // All XAML updates need to occur on the UI thread so dispatch to ensure this is true.
 /*
    Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal,
        ref new DispatchedHandler([this]()
        {
            if (m_licenseInformation->IsActive)
            {
                if (!m_licenseInformation->IsTrial)
                {
//                    PurchaseUpgrade->Visibility = ::Visibility::Collapsed;
                }
            }
            else
            {
                ChangeBackground->Visibility = ::Visibility::Collapsed;
            }

            if (m_licenseInformation->IsActive && m_licenseInformation->IsTrial)
            {
                if (m_listingInformation != nullptr)
                {
  //                  PurchaseMessage->Text =
  //                      "You are running a trial version. Purchase the full version for: " + m_listingInformation->FormattedPrice;
                }
                else
                {
  //                  PurchaseMessage->Text =
  //                      "You are running a trial version. Purchase the full version.";
                }
                if (m_possiblePurchaseUpgrade)
                {
 //                   PurchaseUpgrade->Visibility = ::Visibility::Visible;
                }
            }

            if (m_licenseInformation != nullptr)
            {
                auto items = dynamic_cast<Platform::Collections::Vector<Platform::Object^>^>(ProductListView->ItemsSource);
                for (uint32 i = 0; i < items->Size; i++)
                {
                    dynamic_cast<ProductItem^>(items->GetAt(i))->UpdateContent(m_licenseInformation);
                }
            }
            if (m_listingInformation != nullptr)
            {
                auto items = dynamic_cast<Platform::Collections::Vector<Platform::Object^>^>(ProductListView->ItemsSource);
                for (uint32 i = 0; i < items->Size; i++)
                {
                    dynamic_cast<ProductItem^>(items->GetAt(i))->UpdateContent(m_listingInformation);
                }
            }
        })
        );
        */
}

//----------------------------------------------------------------------

void MainPage::OnBuyAppButtonTapped(Object^ sender, TappedRoutedEventArgs^ args)
{
    args->Handled = true;
    OnBuySelectorClicked(sender, args);
}

//----------------------------------------------------------------------

void MainPage::OnBuySelectorClicked(Object^ sender, RoutedEventArgs^ /* args */)
{
    Platform::String^ tag = dynamic_cast<Platform::String^>(dynamic_cast<Button^>(sender)->CommandParameter);
    //StoreUnavailable->Visibility = ::Visibility::Collapsed;
    /*
    if (tag == "MainApp")
    {
        if ((m_licenseInformation != nullptr) && m_licenseInformation->IsActive)
        {
            if (m_licenseInformation->IsTrial)
            {
#ifdef USE_STORE_SIMULATOR
                task<Platform::String^> purchaseTask(CurrentAppSimulator::RequestAppPurchaseAsync(false));
#else
                task<Platform::String^> purchaseTask(CurrentApp::RequestAppPurchaseAsync(false));
#endif
                purchaseTask.then([this](task<Platform::String^> previousTask)
                {
                    try
                    {
                        // Try getting all exceptions from the continuation chain above this point
                        previousTask.get();
                    }
                    catch (Platform::Exception^ exception)
                    {
                        if (exception->HResult == E_FAIL)
                        {
                            StoreUnavailable->Visibility = ::Visibility::Visible;
                        }
                    }
                });
            }
        }
    }
    else
    {
        if ((m_licenseInformation != nullptr) && m_licenseInformation->IsActive && !m_licenseInformation->IsTrial)
        {
            if (!m_licenseInformation->ProductLicenses->Lookup(tag)->IsActive)
            {
#ifdef USE_STORE_SIMULATOR
                task<Platform::String^> purchaseTask(CurrentAppSimulator::RequestProductPurchaseAsync(tag, false));
#else
                task<Platform::String^> purchaseTask(CurrentApp::RequestProductPurchaseAsync(tag, false));
#endif
                purchaseTask.then([=](task<Platform::String^> previousTask)
                {
                    try
                    {
                        // Try getting all exceptions from the continuation chain above this point
                        previousTask.get();
                    }
                    catch (Platform::Exception^ exception)
                    {
                        if (exception->HResult == E_FAIL)
                        {
//                            StoreUnavailable->Visibility = ::Visibility::Visible;
                        }
                    }
                });
            }
        }
    }
    */
}

//----------------------------------------------------------------------

void MainPage::OnChangeBackgroundButtonClicked(Object^ /* sender */, RoutedEventArgs^ /* args */)
{
    //if ((m_licenseInformation != nullptr) && m_licenseInformation->IsActive)
    //{
    //    if (m_licenseInformation->IsTrial ||
    //        (!m_licenseInformation->ProductLicenses->Lookup("NightBackground")->IsActive  &&
    //        !m_licenseInformation->ProductLicenses->Lookup("DayBackground")->IsActive))
    //    {
    //        ShowStoreFlyout();
    //    }
    //    else
    //    {
    //        m_gameMain->CycleBackground();
    //    }
    //}
}

//----------------------------------------------------------------------

void MainPage::OnResetLicenseButtonClicked(Object^ /* sender */, RoutedEventArgs^ /* args */)
{
//#ifdef USE_STORE_SIMULATOR
//    m_gameMain->ResetLicense();
//#endif
//    m_gameMain->SetBackground(0);
}

//----------------------------------------------------------------------

void MainPage::OptionalTrialUpgrade()
{
  /*
  PurchaseUpgrade->Visibility = ::Visibility::Collapsed;

    if (m_licenseInformation != nullptr)
    {
        if (m_licenseInformation->IsActive && m_licenseInformation->IsTrial)
        {
            if (m_listingInformation != nullptr)
            {
                PurchaseMessage->Text =
                    "You are running a trial version. Purchase the full version for: " + m_listingInformation->FormattedPrice;
            }
            else
            {
                PurchaseMessage->Text =
                    "You are running a trial version. Purchase the full version.";
            }
            PurchaseUpgrade->Visibility = ::Visibility::Visible;
        }
    }
    */
}

//----------------------------------------------------------------------

void MainPage::OnStoreReturnClicked(Object^ /* sender */, RoutedEventArgs^ /* args */)
{
//    StoreFlyout->IsOpen = false;
//    StoreFlyout->Visibility = ::Visibility::Collapsed;
}

//----------------------------------------------------------------------

void MainPage::OnLoadStoreClicked(Object^ /* sender */, RoutedEventArgs^ /* args */)
{
//    m_gameMain->PauseRequested();
//    ShowStoreFlyout();
}

//----------------------------------------------------------------------

void MainPage::SetProductItems(
    ListingInformation^ listing,
    LicenseInformation^ license
    )
{
    auto items = ref new Platform::Collections::Vector<Platform::Object^>();
    items->Append(ref new ProductItem(listing, license, "MainApp", true));
    items->Append(ref new ProductItem(listing, license, "AutoFire", false));
    items->Append(ref new ProductItem(listing, license, "NightBackground", false));
    items->Append(ref new ProductItem(listing, license, "DayBackground", false));
//    ProductListView->ItemsSource = items;
//    StoreUnavailable->Visibility = ::Visibility::Collapsed;
}

//----------------------------------------------------------------------

void MainPage::OnWindowSizeChanged()
{
///    StoreGrid->Height = Window::Current->Bounds.Height;
//    StoreFlyout->HorizontalOffset = Window::Current->Bounds.Width - StoreGrid->Width;
}

//----------------------------------------------------------------------

void MainPage::OnAppBarOpened(Object^ /* sender */, Object^ /* args */)
{
      m_gameMain->GetStateMachine().process_event(stm::ev::ShowAppMenu(m_gameMain));
      //m_gameMain->PauseRequested();
}

//----------------------------------------------------------------------

void MainPage::ShowStoreFlyout()
{
 /*
  StoreGrid->Height = Window::Current->Bounds.Height;
    StoreUnavailable->Visibility = ::Visibility::Collapsed;
    StoreFlyout->HorizontalOffset = Window::Current->Bounds.Width - StoreGrid->Width;
    StoreFlyout->IsOpen = true;
    StoreFlyout->Visibility = ::Visibility::Visible;*/
    GameAppBar->IsOpen = false;
}

//----------------------------------------------------------------------


void sf::MainPage::GameAppBar_Closed(Platform::Object^ sender, Platform::Object^ e)
{

  m_gameMain->GetStateMachine().process_event(stm::ev::HideAppMenu(m_gameMain));
}


void sf::MainPage::KillPlayer_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

  m_gameMain->GetStateMachine().process_event(stm::ev::PlayerIsGone(m_gameMain));
}


void sf::MainPage::ReturnMenu_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
  m_gameMain->GetStateMachine().process_event(stm::ev::ReturnMenu(m_gameMain));
}


void sf::MainPage::m_PlayerKill_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

  m_gameMain->GetStateMachine().process_event(stm::ev::PlayerIsGone(m_gameMain));

}
