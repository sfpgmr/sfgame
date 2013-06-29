//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "Common\SuspensionManager.h"
#include "ConstantBuffers.h"
#include "MenuPage.xaml.h"
#include "MainPage.xaml.h"
#include "GameRenderer.h"
#include "sfmf.h"
#include "DirectXSample.h"
#include "Level1.h"
#include "Animate.h"
//#include "Sphere.h"
//#include "Cylinder.h"
//#include "Face.h"
#include "MediaReader.h"
extern void testout();

using namespace sf::Common;
using namespace Platform;
using namespace concurrency;
using namespace DirectX;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Store;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::Storage;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Interop;

//----------------------------------------------------------------------
namespace sf {
  GameMain::GameMain():
    m_gameActive(false),
    m_levelActive(false),
    m_levelCount(0),
    m_currentLevel(0),
    m_activeBackground(0),m_processTime(0.0f)
  {
    game_stm_.start();
  }

  //----------------------------------------------------------------------

  void GameMain::Initialize(
    _In_ MoveLookController^ controller,
    _In_ GameRenderer^ renderer
    )
  {
    // This method is expected to be called as an asynchronous task.
    // Care should be taken to not call rendering methods on the
    // m_renderer as this would result in the D3D Context being
    // used in multiple threads, which is not allowed.

    m_controller = controller;
    m_renderer = renderer;

    m_audioController = ref new Audio;
    m_audioController->CreateDeviceIndependentResources();

    InitializeGameConfig();

    //m_ammo = std::vector<Sphere^>(GameConstants::MaxAmmo);
    m_objects = std::vector<GameObject^>();
    m_renderObjects = std::vector<GameObject^>();
    m_level = std::vector<Level^>();

    m_savedState = ref new PersistentState();
    m_savedState->Initialize(ApplicationData::Current->LocalSettings->Values, "Game");

    m_timer = ref new GameTimer();


    m_camera = ref new Camera;
    m_camera->SetProjParams(XM_PI / 2, 1.0f, 0.01f, 100.0f);
    m_camera->SetViewParams(
      XMFLOAT3 (0.0f, -1.3f, 4.0f),            // Eye point in world coordinates.
      XMFLOAT3 (0.0f, 0.7f, 0.0f),     // Look at point in world coordinates.
      XMFLOAT3 (0.0f, 1.0f, 0.0f)      // The Up vector for the camera.
      );

    m_controller->Pitch(m_camera->Pitch());
    m_controller->Yaw(m_camera->Yaw());

    //MediaReader^ mediaReader = ref new MediaReader;
    //auto targetHitSound = mediaReader->LoadMedia("hit.wav");

    // Instantiate a set of spheres to be used as ammunition for the game
    // and set the material properties of the spheres.
    // auto ammoHitSound = mediaReader->LoadMedia("bounce.wav");

    // Instantiate each of the game levels. The Level class contains methods
    // that initialize the objects in the world for the given level and also
    // define any motion paths for the objects in that level.

    m_level.push_back(ref new Level1);
    m_levelCount = static_cast<uint32>(m_level.size());

    // Load the top score from disk if it exists.
    LoadHighScore();

    // Load the currentScore for saved state if it exists.
    LoadState();

    m_controller->Active(false);
  }

  //----------------------------------------------------------------------

  void GameMain::LoadGame()
  {
    m_currentLevel = 0;
    m_gameActive = false;
    m_levelActive = false;
    m_timer->Reset();
  }

  //----------------------------------------------------------------------

  task<void> GameMain::LoadLevelAsync()
  {
    // Initialize the level and spin up the async loading of the rendering
    // resources for the level.
    // This will run in a separate thread, so for Direct3D 11, only Device
    // methods are allowed.  Any DeviceContext method calls need to be
    // done in FinalizeLoadLevel.

    m_level[m_currentLevel]->Initialize(m_objects);
    return m_renderer->LoadLevelResourcesAsync();
  }

  //----------------------------------------------------------------------

  void GameMain::FinalizeLoadLevel()
  {
    // This method is called on the main thread, so Direct3D 11 DeviceContext
    // method calls are allowable here.

    // Finalize the Level loading.
    m_renderer->FinalizeLoadLevelResources();
  }

  //----------------------------------------------------------------------

  void GameMain::StartLevel()
  {
    m_mainPage->MainFrame->Visibility = Visibility::Collapsed;
    m_game.init();
    playerLeft = 3;// 伝統のデフォルト３機！
    m_timer->Reset();
    m_timer->Start();
    if (m_currentLevel == 0)
    {
      m_gameActive = true;
    }
    m_levelActive = true;
    m_controller->Active(true);
  }
  void GameMain::ShowMenu()
  {
    m_mainPage->MainFrame->Visibility = Visibility::Visible;
  }

  void GameMain::RestartLevel()
  {
    m_timer->Start();
  }


  //----------------------------------------------------------------------

  void GameMain::PauseGame()
  {
    m_timer->Stop();
    m_renderer->RenderPause();
    SaveState();
  }

  //----------------------------------------------------------------------

  void GameMain::ContinueGame()
  {
    m_timer->Start();
    m_controller->Active(true);
  }

  //----------------------------------------------------------------------

  void GameMain::RunGame()
  {
    // This method is called to execute a single time interval for active game play.
    // It returns the resulting state of game play after the interval has been executed.
    m_timer->Update();
    m_controller->Update();
    // Time has not expired, so run one frame of game play.
    UpdateDynamics();
    m_renderer->Render();
  }

  //----------------------------------------------------------------------

  void GameMain::OnSuspending()
  {
    m_audioController->SuspendAudio();
  }

  //----------------------------------------------------------------------

  void GameMain::OnResuming()
  {
    m_audioController->ResumeAudio();
  }

  //----------------------------------------------------------------------

  void GameMain::UpdateDynamics()
  {
    float timeFrame = m_timer->DeltaTime();
    float start = m_timer->Now();
    m_game.step(timeFrame);
    m_processTime = (m_processTime + (m_timer->Now() - start)) / 2.0f;
  }

  //----------------------------------------------------------------------

  void GameMain::SaveState()
  {
    // Save basic state of the game.
    m_savedState->SaveBool(":GameActive", m_gameActive);
    m_savedState->SaveBool(":LevelActive", m_levelActive);
    m_savedState->SaveInt32(":LevelCompleted", m_currentLevel);
    m_savedState->SaveXMFLOAT3(":PlayerLookDirection", m_controller->LookDirection());

    if (m_levelActive)
    {
      // The game is currently in the middle of a level, so save the extended state of 
      // the game.
      m_savedState->SaveSingle(":LevelPlayingTime", m_timer->PlayingTime());
      m_level[m_currentLevel]->SaveState(m_savedState);
    }
  }

  //----------------------------------------------------------------------

  void GameMain::LoadState()
  {
    m_gameActive = m_savedState->LoadBool(":GameActive", m_gameActive);
    m_levelActive = m_savedState->LoadBool(":LevelActive", m_levelActive);

    if (m_gameActive)
    {
      // Loading from the last known state means the game wasn't finished when it was last played,
      // so set the current level.

      m_currentLevel = m_savedState->LoadInt32(":LevelCompleted", 0);

    }
    else
    {
      // The game was not being played when it was last saved, so initialize to the beginning.
      m_currentLevel = 0;
    }

    if (m_currentLevel >= m_levelCount)
    {
      // The current level is not valid so, reset to a known state and abandon the current game.
      m_currentLevel = 0;
      m_gameActive = false;
      m_levelActive = false;
    }
  }

  //----------------------------------------------------------------------

  void GameMain::SetCurrentLevelToSavedState()
  {
    if (m_gameActive)
    {
      if (m_levelActive)
      {
        // Middle of a level so restart where left off.
        m_timer->Reset();
        m_timer->PlayingTime(m_savedState->LoadSingle(":LevelPlayingTime", 0.0f));
        m_level[m_currentLevel]->LoadState(m_savedState);
      }
    }
  }

  //----------------------------------------------------------------------

  void GameMain::SaveHighScore()
  {
    for(int i = 0;i < HIGHSCORE_SIZE;++i){
      m_savedState->SaveString(":HighScore:Name", m_highScoreEntries[i].Name);
      m_savedState->SaveInt32(":HighScore:Score", m_highScoreEntries[i].Score);
    }
  }

  //----------------------------------------------------------------------

  void GameMain::LoadHighScore()
  {
    for(int i = 0;i < HIGHSCORE_SIZE;++i)
    {
      m_highScoreEntries[i].Name =  m_savedState->LoadString(":HighScore:Name","");
      m_highScoreEntries[i].Score = m_savedState->LoadInt32(":HighScore:Score", 0);
    }
  }

  //----------------------------------------------------------------------

  void GameMain::InitializeGameConfig()
  {
    m_gameConfig.isTrial = true;
    m_gameConfig.autoFire = false;
    m_controller->AutoFire(false);
    m_activeBackground = 0;
    m_gameConfig.backgroundAvailable[0] = true;
    for (int i = 1; i < GameConstants::MaxBackgroundTextures; i++)
    {
      m_gameConfig.backgroundAvailable[i] = false;
    }
  }

  //--------------------------------------------------------------------------------------

  void GameMain::UpdateGameConfig(LicenseInformation^ licenseInformation)
  {
    if (licenseInformation->IsActive)
    {
      m_gameConfig.isTrial = licenseInformation->IsTrial;
      if (!m_gameConfig.isTrial && licenseInformation->ProductLicenses->Lookup("AutoFire")->IsActive)
      {
        m_gameConfig.autoFire = true;
        m_controller->AutoFire(true);
      }
      else
      {
        m_gameConfig.autoFire = false;
        m_controller->AutoFire(false);
      }
      if (!m_gameConfig.isTrial && licenseInformation->ProductLicenses->Lookup("NightBackground")->IsActive)
      {
        m_gameConfig.backgroundAvailable[1] = true;
      }
      else
      {
        m_gameConfig.backgroundAvailable[1] = false;
      }
      if (!m_gameConfig.isTrial && licenseInformation->ProductLicenses->Lookup("DayBackground")->IsActive)
      {
        m_gameConfig.backgroundAvailable[2] = true;
      }
      else
      {
        m_gameConfig.backgroundAvailable[2] = false;
      }
    }
    else
    {
      // If no active license then default back to trial version.
      InitializeGameConfig();
    }

    if (m_gameConfig.isTrial)
    {
      if (m_level.size() > 2)
      {
        m_level.erase(m_level.begin() + 2, m_level.end());
      }
    }
    else
    {
    }
    m_levelCount = static_cast<uint32>(m_level.size());
  }

  void GameMain::ActionOnLaunched()
  {
    m_mainPage = dynamic_cast<MainPage^>(Window::Current->Content);


    // ウィンドウに既にコンテンツが表示されている場合は、アプリケーションの初期化を繰り返さずに、
    // ウィンドウがアクティブであることだけを確認してください
    if (m_mainPage == nullptr)
    {
      // ナビゲーション コンテキストとして動作するフレームを作成し、
      // SuspensionManager キーに関連付けます
      m_mainPage = ref new MainPage();
      m_mainPage->SetApp(this);
      SuspensionManager::RegisterFrame(m_mainPage->MainFrame,L"MainFrame");

      auto prerequisite = task<void>([](){});
      if (previousExecutionState == ApplicationExecutionState::Terminated)
      {
        // 必要な場合のみ、保存されたセッション状態を復元し、
        // 復元完了後の最後の起動手順をスケジュールします
        prerequisite = SuspensionManager::RestoreAsync();
      }
      prerequisite.then([=](task<void> prerequisite)
      {
        try
        {
          prerequisite.get();
        }
        catch (Platform::Exception^)
        {
          //状態の復元に何か問題があります。
          //状態がないものとして続行します
        }

        if (m_mainPage->MainFrame->Content == nullptr)
        {
          // ナビゲーション スタックが復元されていない場合、最初のページに移動します。
          // このとき、必要な情報をナビゲーション パラメーターとして渡して、新しいページを
          // を構成します
          if (!m_mainPage->MainFrame->Navigate(TypeName(MenuPage::typeid),this))
          {
            throw ref new FailureException("Failed to create initial page");
          }
        }
        // フレームを現在のウィンドウに配置します
        Window::Current->Content = m_mainPage;
        Window::Current->Activated += ref new WindowActivatedEventHandler(this, &GameMain::OnWindowActivationChanged);

        // 現在のウィンドウがアクティブであることを確認します
        Window::Current->Activate();

      }, task_continuation_context::use_current());
    }
    else
    {
      if (m_mainPage->MainFrame->Content == nullptr)
      {
        // ナビゲーション スタックが復元されていない場合、最初のページに移動します。
        // このとき、必要な情報をナビゲーション パラメーターとして渡して、新しいページを
        // 構成します
        if (!m_mainPage->MainFrame->Navigate(TypeName(MenuPage::typeid), this))
        {
          throw ref new FailureException("Failed to create initial page");
        }
      }
      // 現在のウィンドウがアクティブであることを確認します
      Window::Current->Activated += ref new WindowActivatedEventHandler(this, &GameMain::OnWindowActivationChanged);
      Window::Current->Activate();
    }

    //   Suspending += ref new SuspendingEventHandler(this, &App::OnSuspending);
    //    Resuming += ref new EventHandler<Object^>(this, &App::OnResuming);

    m_controller = ref new MoveLookController();
    m_renderer = ref new GameRenderer();

    auto window = Window::Current->CoreWindow;

    PointerVisualizationSettings^ visualizationSettings = PointerVisualizationSettings::GetForCurrentView();
    visualizationSettings->IsContactFeedbackEnabled = false;
    visualizationSettings->IsBarrelButtonFeedbackEnabled = false;

    window->SizeChanged +=
      ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &GameMain::OnWindowSizeChanged);

    window->VisibilityChanged +=
      ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &GameMain::OnVisibilityChanged);

    DisplayProperties::LogicalDpiChanged +=
      ref new DisplayPropertiesEventHandler(this, &GameMain::OnLogicalDpiChanged);

    DisplayProperties::DisplayContentsInvalidated +=
      ref new DisplayPropertiesEventHandler(this, &GameMain::OnDisplayContentsInvalidated);

       window->KeyDown +=
        ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &GameMain::OnKeyDown);
    m_controller->Initialize(window);
    m_controller->AutoFire(false);

    m_controller->SetMoveRect(
      XMFLOAT2(0.0f, window->Bounds.Height - GameConstants::TouchRectangleSize),
      XMFLOAT2(GameConstants::TouchRectangleSize, window->Bounds.Height)
      );
    m_controller->SetFireRect(
      XMFLOAT2(window->Bounds.Width - GameConstants::TouchRectangleSize, window->Bounds.Height - GameConstants::TouchRectangleSize),
      XMFLOAT2(window->Bounds.Width, window->Bounds.Height)
      );

    m_renderer->Initialize(window, m_mainPage->GetSwapChainBackgroundPanel(), DisplayProperties::LogicalDpi);
    m_renderer->DeviceLost += ref new D3DDeviceEventHandler(this, &GameMain::OnDeviceLost);
    m_renderer->DeviceReset += ref new D3DDeviceEventHandler(this, &GameMain::OnDeviceReset);

    //    SetGameInfoOverlay(GameInfoOverlayState::Loading);
    //    SetAction(GameInfoOverlayCommand::None);
    //   //ShowGameInfoOverlay();

    m_onRenderingEventToken = CompositionTarget::Rendering::add(ref new EventHandler<Object^>(this, &GameMain::OnRendering));
    m_renderNeeded = true;

    create_task([this]()
    {
      // Asynchronously initialize the game class and load the renderer device resources.
      // By doing all this asynchronously, the game gets to its main loop more quickly
      // and in parallel all the necessary resources are loaded on other threads.
      Initialize(m_controller, m_renderer);

      return m_renderer->CreateGameDeviceResourcesAsync(this);

    }).then([this]()
    {
      // The finalize code needs to run in the same thread context
      // as the m_renderer object was created because the D3D device context
      // can ONLY be accessed on a single thread.
      m_renderer->FinalizeCreateGameDeviceResources();

      InitializeLicense();
      //        InitializeGameState();

      GetStateMachine().process_event(stm::ev::InitComplete(this));
      //if (m_updateState == UpdateEngineState::WaitingForResources)
      //{
      //    // In the middle of a game so spin up the async task to load the level.
      //    create_task([this]()
      //    {
      //        return m_game->LoadLevelAsync();

      //    }).then([this]()
      //    {
      //        // The m_game object may need to deal with D3D device context work so
      //        // again the finalize code needs to run in the same thread
      //        // context as the m_renderer object was created because the D3D
      //        // device context can ONLY be accessed on a single thread.
      //        m_game->FinalizeLoadLevel();
      //        m_game->SetCurrentLevelToSavedState();
      //        m_updateState = UpdateEngineState::ResourcesLoaded;

      //    }, task_continuation_context::use_current());
      //}
    }, task_continuation_context::use_current());
  }

  //----------------------------------------------------------------------

  void GameMain::OnRendering(
    _In_ Object^ /* sender */,
    _In_ Object^ /* args */
    )
  {
    GetStateMachine().process_event(stm::ev::Update(this));

    //Update();

    //if (m_updateState == UpdateEngineState::Dynamics || m_renderNeeded)
    //{
    //    m_renderer->Render();
    //    m_renderNeeded = false;
    //}

    //if (!m_visible || !m_haveFocus || (m_updateState == UpdateEngineState::Snapped))
    //{
    //    // App is in an inactive state so disable the OnRendering callback
    //    // This optimizes for power and allows the framework to become more queiecent
    //    if (m_onRenderingEventToken.Value != 0)
    //    {
    //        CompositionTarget::Rendering::remove(m_onRenderingEventToken);
    //        m_onRenderingEventToken.Value = 0;
    //    }
    //}
  }

  //--------------------------------------------------------------------------------------

  void GameMain::OnWindowSizeChanged(
    _In_ CoreWindow^ window,
    _In_ WindowSizeChangedEventArgs^ /* args */
    )
  {
    UpdateViewState();
    m_mainPage->OnWindowSizeChanged();
    m_renderer->UpdateForWindowSizeChange();

    // The location of the Control regions may have changed with the size change so update the controller.
    m_controller->SetMoveRect(
      XMFLOAT2(0.0f, window->Bounds.Height - GameConstants::TouchRectangleSize),
      XMFLOAT2(GameConstants::TouchRectangleSize, window->Bounds.Height)
      );
    m_controller->SetFireRect(
      XMFLOAT2(window->Bounds.Width - GameConstants::TouchRectangleSize, window->Bounds.Height - GameConstants::TouchRectangleSize),
      XMFLOAT2(window->Bounds.Width, window->Bounds.Height)
      );

    m_renderNeeded = true;

    if (m_onRenderingEventToken.Value == 0)
    {
      // Add the OnRendering Callback to make sure that an update happens.
      m_onRenderingEventToken = CompositionTarget::Rendering::add(ref new EventHandler<Object^>(this, &GameMain::OnRendering));
    }
  }

  //--------------------------------------------------------------------------------------

  void GameMain::OnLogicalDpiChanged(
    _In_ Object^ /* sender */
    )
  {
    m_renderer->SetDpi(DisplayProperties::LogicalDpi);
  }

  //--------------------------------------------------------------------------------------

  void GameMain::OnDisplayContentsInvalidated(
    _In_ Object^ /* sender */
    )
  {
    m_renderNeeded = true;

    if (m_onRenderingEventToken.Value == 0)
    {
      // Add the OnRendering Callback to make sure that an update happens.
      m_onRenderingEventToken = CompositionTarget::Rendering::add(ref new EventHandler<Object^>(this, &GameMain::OnRendering));
    }
  }

  //--------------------------------------------------------------------------------------

  void GameMain::OnVisibilityChanged(
    _In_ CoreWindow^ sender,
    _In_ VisibilityChangedEventArgs^ args
    )
  {
    m_visible = args->Visible;

    if (m_visible && (m_onRenderingEventToken.Value == 0))
    {
      // App is now visible so set up the event handler to do game processing and rendering
      m_onRenderingEventToken = CompositionTarget::Rendering::add(ref new EventHandler<Object^>(this, &GameMain::OnRendering));
    }
  }

  //--------------------------------------------------------------------------------------



  //--------------------------------------------------------------------------------------

  void GameMain::Update()
  {
    m_controller->Update();
    // TODO: このSwitch文をMSM化する
    //switch (m_updateState)
    //{
    //case UpdateEngineState::ResourcesLoaded:
    //    switch (m_pressResult)
    //    {
    //    case PressResultState::LoadGame:
    //        SetGameInfoOverlay(GameInfoOverlayState::GameStats);
    //        break;

    //    case PressResultState::PlayLevel:
    //        SetGameInfoOverlay(GameInfoOverlayState::LevelStart);
    //        break;

    //    case PressResultState::ContinueLevel:
    //        SetGameInfoOverlay(GameInfoOverlayState::Pause);
    //        break;
    //    }
    //    m_updateState = UpdateEngineState::WaitingForPress;
    //    SetAction(GameInfoOverlayCommand::TapToContinue);
    //    m_controller->WaitForPress();
    //    //ShowGameInfoOverlay();
    //    m_renderNeeded = true;
    //    break;

    //case UpdateEngineState::WaitingForPress:
    //    if (m_controller->IsPressComplete() || m_pressComplete)
    //    {
    //        m_pressComplete = false;

    //        switch (m_pressResult)
    //        {
    //        case PressResultState::LoadGame:
    //            m_updateState = UpdateEngineState::WaitingForResources;
    //            m_pressResult = PressResultState::PlayLevel;
    //            m_controller->Active(false);
    //            m_game->LoadGame();
    //            SetAction(GameInfoOverlayCommand::PleaseWait);
    //            SetGameInfoOverlay(GameInfoOverlayState::LevelStart);
    //            //ShowGameInfoOverlay();
    //            m_renderNeeded = true;

    //            m_game->LoadLevelAsync().then([this]()
    //            {
    //                m_game->FinalizeLoadLevel();
    //                m_updateState = UpdateEngineState::ResourcesLoaded;
    //                m_renderNeeded = true;
    //            }, task_continuation_context::use_current());
    //            break;

    //        case PressResultState::PlayLevel:
    //            m_updateState = UpdateEngineState::Dynamics;
    //            HideGameInfoOverlay();
    //            m_controller->Active(true);
    //            m_game->StartLevel();
    //            break;

    //        case PressResultState::ContinueLevel:
    //            m_updateState = UpdateEngineState::Dynamics;
    //            HideGameInfoOverlay();
    //            m_controller->Active(true);
    //            m_game->ContinueGame();
    //            break;
    //        }
    //    }
    //    break;

    //case UpdateEngineState::Dynamics:
    //    if (m_controller->IsPauseRequested() || m_pauseRequested)
    //    {
    //        m_pauseRequested = false;

    //        m_game->PauseGame();
    //        SetGameInfoOverlay(GameInfoOverlayState::Pause);
    //        SetAction(GameInfoOverlayCommand::TapToContinue);
    //        m_updateState = UpdateEngineState::WaitingForPress;
    //        m_pressResult = PressResultState::ContinueLevel;
    //        //ShowGameInfoOverlay();
    //        m_renderNeeded = true;
    //    }
    //    else
    //    {
    //        GameState runState = m_game->RunGame();
    //        switch (runState)
    //        {
    //        case GameState::TimeExpired:
    //            SetAction(GameInfoOverlayCommand::TapToContinue);
    //            SetGameInfoOverlay(GameInfoOverlayState::GameOverExpired);
    //            //ShowGameInfoOverlay();
    //            m_updateState = UpdateEngineState::WaitingForPress;
    //            m_pressResult = PressResultState::LoadGame;
    //            m_renderNeeded = true;
    //            break;

    //        case GameState::LevelComplete:
    //            SetAction(GameInfoOverlayCommand::PleaseWait);
    //            SetGameInfoOverlay(GameInfoOverlayState::LevelStart);
    //            //ShowGameInfoOverlay();
    //            m_updateState = UpdateEngineState::WaitingForResources;
    //            m_pressResult = PressResultState::PlayLevel;
    //            m_renderNeeded = true;

    //            m_game->LoadLevelAsync().then([this]()
    //            {
    //                m_game->FinalizeLoadLevel();
    //                m_updateState = UpdateEngineState::ResourcesLoaded;
    //                m_renderNeeded = true;

    //            }, task_continuation_context::use_current());
    //            break;

    //        case GameState::GameComplete:
    //            SetAction(GameInfoOverlayCommand::TapToContinue);
    //            SetGameInfoOverlay(GameInfoOverlayState::GameOverCompleted);
    //            //ShowGameInfoOverlay();
    //            m_updateState  = UpdateEngineState::WaitingForPress;
    //            m_pressResult = PressResultState::LoadGame;
    //            m_renderNeeded = true;
    //            break;
    //        }
    //    }

    //    if (m_updateState == UpdateEngineState::WaitingForPress)
    //    {
    //        // Transitioning state, so enable waiting for the press event
    //        m_controller->WaitForPress();
    //    }
    //    if (m_updateState == UpdateEngineState::WaitingForResources)
    //    {
    //        // Transitioning state, so shut down the input controller until resources are loaded
    //        m_controller->Active(false);
    //    }
    //    break;
    //}
  }

  //--------------------------------------------------------------------------------------

  void GameMain::OnWindowActivationChanged(
    _In_ Platform::Object^ /* sender */,
    _In_ Windows::UI::Core::WindowActivatedEventArgs^ args
    )
  {
    if (args->WindowActivationState == CoreWindowActivationState::Deactivated)
    {
      m_haveFocus = false;

      //switch (m_updateState)
      //{
      //case UpdateEngineState::Dynamics:
      //    // From Dynamic mode, when coming out of Deactivated rather than going directly back into game play
      //    // go to the paused state waiting for user input to continue
      //    m_updateStateNext = UpdateEngineState::WaitingForPress;
      //    m_pressResult = PressResultState::ContinueLevel;
      //    SetGameInfoOverlay(GameInfoOverlayState::Pause);
      //    //ShowGameInfoOverlay();
      //    m_game->PauseGame();
      //    m_updateState = UpdateEngineState::Deactivated;
      //    SetAction(GameInfoOverlayCommand::None);
      //    m_renderNeeded = true;
      //    break;

      //case UpdateEngineState::WaitingForResources:
      //case UpdateEngineState::WaitingForPress:
      //    m_updateStateNext = m_updateState;
      //    m_updateState = UpdateEngineState::Deactivated;
      //    SetAction(GameInfoOverlayCommand::None);
      //    //ShowGameInfoOverlay();
      //    m_renderNeeded = true;
      //    break;
      //}
      // Don't have focus so shutdown input processing
      m_controller->Active(false);
    }
    else if (args->WindowActivationState == CoreWindowActivationState::CodeActivated
      || args->WindowActivationState == CoreWindowActivationState::PointerActivated)
    {
      m_haveFocus = true;

      //if (m_updateState == UpdateEngineState::Deactivated)
      //{
      //    m_updateState = m_updateStateNext;

      //    if (m_updateState == UpdateEngineState::WaitingForPress)
      //    {
      //        SetAction(GameInfoOverlayCommand::TapToContinue);
      //        m_controller->WaitForPress();
      //    }
      //    else if (m_updateStateNext == UpdateEngineState::WaitingForResources)
      //    {
      //        SetAction(GameInfoOverlayCommand::PleaseWait);
      //    }

      //    // App is now "active" so set up the event handler to do game processing and rendering
      //    if (m_onRenderingEventToken.Value == 0)
      //    {
      //        m_onRenderingEventToken = CompositionTarget::Rendering::add(ref new EventHandler<Object^>(this, &App::OnRendering));
      //    }
      //}
    }
  }

  //--------------------------------------------------------------------------------------

  void GameMain::OnSuspending(
    _In_ Platform::Object^ /* sender */,
    _In_ SuspendingEventArgs^ args
    )
  {
    // Save application state.
    //switch (m_updateState)
    //{
    //case UpdateEngineState::Dynamics:
    //    // Game is in the active game play state, Stop Game Timer and Pause play and save state
    //    SetAction(GameInfoOverlayCommand::None);
    //    SetGameInfoOverlay(GameInfoOverlayState::Pause);
    //    m_updateStateNext = UpdateEngineState::WaitingForPress;
    //    m_pressResult = PressResultState::ContinueLevel;
    //    m_game->PauseGame();
    //    break;

    //case UpdateEngineState::WaitingForResources:
    //case UpdateEngineState::WaitingForPress:
    //    m_updateStateNext = m_updateState;
    //    break;

    //default:
    //    // any other state don't save as next state as they are trancient states and have already set m_updateStateNext
    //    break;
    //}
    //m_updateState = UpdateEngineState::Suspended;

    m_controller->Active(false);
    OnSuspending();
  }

  //--------------------------------------------------------------------------------------

  void GameMain::OnResuming(
    _In_ Platform::Object^ /* sender */,
    _In_ Platform::Object^ /* args */
    )
  {
    //if (m_haveFocus)
    //{
    //    m_updateState = m_updateStateNext;
    //}
    //else
    //{
    //    m_updateState = UpdateEngineState::Deactivated;
    //}

    //if (m_updateState == UpdateEngineState::WaitingForPress)
    //{
    //    SetAction(GameInfoOverlayCommand::TapToContinue);
    //    m_controller->WaitForPress();
    //}
    OnResuming();
    //ShowGameInfoOverlay();
    m_renderNeeded = true;
  }

  //--------------------------------------------------------------------------------------

  void GameMain::OnDeviceLost()
  {
    //if (m_updateState == UpdateEngineState::Dynamics)
    //{
    //    // Game is in the active game play state, Stop Game Timer and Pause play and save state
    //    m_game->PauseGame();
    //}
    m_renderNeeded = false;
  }

  //--------------------------------------------------------------------------------------

  void GameMain::OnDeviceReset()
  {

    //SetAction(GameInfoOverlayCommand::PleaseWait);
    //SetGameInfoOverlay(GameInfoOverlayState::Loading);
    //m_updateState = UpdateEngineState::WaitingForResources;
    m_renderNeeded = true;

    if (m_onRenderingEventToken.Value == 0)
    {
      // Add the OnRendering Callback to make sure that an update happens.
      m_onRenderingEventToken = CompositionTarget::Rendering::add(ref new EventHandler<Object^>(this, &GameMain::OnRendering));
    }

    create_task([this]()
    {
      return m_renderer->CreateGameDeviceResourcesAsync(this);

    }).then([this]()
    {
      // The finalize code needs to run in the same thread context
      // as the m_renderer object was created because the D3D device context
      // can ONLY be accessed on a single thread.
      m_renderer->FinalizeCreateGameDeviceResources();

      // Reset the main state machine based on the current game state now
      // that the device resources have been restored.
      //      InitializeGameState();

      //if (m_updateState == UpdateEngineState::WaitingForResources)
      //{
      //    // In the middle of a game so spin up the async task to load the level.
      //    return m_game->LoadLevelAsync().then([this]()
      //    {
      //        // The m_game object may need to deal with D3D device context work so
      //        // again the finalize code needs to run in the same thread
      //        // context as the m_renderer object was created because the D3D
      //        // device context can ONLY be accessed on a single thread.
      //        m_game->FinalizeLoadLevel();
      //        m_game->SetCurrentLevelToSavedState();
      //        m_updateState = UpdateEngineState::ResourcesLoaded;

      //    }, task_continuation_context::use_current());
      //}
      //else
      //{
      //    // The game is not in the middle of a level so there aren't any level
      //    // resources to load.  Creating a no-op task so that in both cases
      //    // the same continuation logic is used.
      //    return create_task([]()
      //    {
      //    });
      //}

    }, task_continuation_context::use_current()).then([this]()
    {
      // Since Device lost is an unexpected event, the app visual state
      // may be snapped or not have focus.  Put the state machine
      // into the correct state to reflect these cases.
      //if (ApplicationView::Value == ApplicationViewState::Snapped)
      //{
      //    m_updateStateNext = m_updateState;
      //    m_updateState = UpdateEngineState::Snapped;
      //    m_controller->Active(false);
      //    HideGameInfoOverlay();
      //    SetSnapped();
      //    m_renderNeeded = true;
      //}
      //else if (!m_haveFocus)
      //{
      //    m_updateStateNext = m_updateState;
      //    m_updateState = UpdateEngineState::Deactivated;
      //    m_controller->Active(false);
      //    SetAction(GameInfoOverlayCommand::None);
      //    m_renderNeeded = true;
      //}

      if (m_onRenderingEventToken.Value == 0)
      {
        // Add the OnRendering Callback to make sure that an update happens.
        m_onRenderingEventToken = CompositionTarget::Rendering::add(ref new EventHandler<Object^>(this, &GameMain::OnRendering));
      }

    }, task_continuation_context::use_current());
  }

  //--------------------------------------------------------------------------------------

  void GameMain::UpdateViewState()
  {
    //if (ApplicationView::Value == ApplicationViewState::Snapped)
    //{
    //    switch (m_updateState)
    //    {
    //    case UpdateEngineState::Dynamics:
    //        // From Dynamic mode, when coming out of SNAPPED layout rather than going directly back into game play
    //        // go to the paused state waiting for user input to continue
    //        m_updateStateNext = UpdateEngineState::WaitingForPress;
    //        m_pressResult = PressResultState::ContinueLevel;
    //        SetGameInfoOverlay(GameInfoOverlayState::Pause);
    //        SetAction(GameInfoOverlayCommand::TapToContinue);
    //        m_game->PauseGame();
    //        break;

    //    case UpdateEngineState::WaitingForResources:
    //    case UpdateEngineState::WaitingForPress:
    //        // Avoid corrupting the m_updateStateNext on a transition from Snapped -> Snapped.
    //        // Otherwise just cache the current state and return to it when leaving SNAPPED layout

    //        m_updateStateNext = m_updateState;
    //        break;

    //    default:
    //        break;
    //    }

    //    m_updateState = UpdateEngineState::Snapped;
    //    m_controller->Active(false);
    //    HideGameInfoOverlay();
    //    SetSnapped();
    //    m_renderNeeded = true;
    //}
    //else if (ApplicationView::Value == ApplicationViewState::Filled ||
    //    ApplicationView::Value == ApplicationViewState::FullScreenLandscape ||
    //    ApplicationView::Value == ApplicationViewState::FullScreenPortrait)
    //{
    //    if (m_updateState == UpdateEngineState::Snapped)
    //    {
    //        HideSnapped();
    //        //ShowGameInfoOverlay();
    //        m_renderNeeded = true;

    //        if (m_haveFocus)
    //        {
    //            if (m_updateStateNext == UpdateEngineState::WaitingForPress)
    //            {
    //                SetAction(GameInfoOverlayCommand::TapToContinue);
    //                m_controller->WaitForPress();
    //            }
    //            else if (m_updateStateNext == UpdateEngineState::WaitingForResources)
    //            {
    //                SetAction(GameInfoOverlayCommand::PleaseWait);
    //            }

    //            m_updateState = m_updateStateNext;
    //        }
    //        else
    //        {
    //            m_updateState = UpdateEngineState::Deactivated;
    //            SetAction(GameInfoOverlayCommand::None);
    //        }
    //    }
    //    else if (m_updateState == UpdateEngineState::Dynamics)
    //    {
    //        // In Dynamic mode, when a resize event happens, go to the paused state waiting for user input to continue.
    //        m_pressResult = PressResultState::ContinueLevel;
    //        SetGameInfoOverlay(GameInfoOverlayState::Pause);
    //        m_game->PauseGame();
    //        if (m_haveFocus)
    //        {
    //            m_updateState = UpdateEngineState::WaitingForPress;
    //            SetAction(GameInfoOverlayCommand::TapToContinue);
    //            m_controller->WaitForPress();
    //        }
    //        else
    //        {
    //            m_updateState = UpdateEngineState::Deactivated;
    //            SetAction(GameInfoOverlayCommand::None);
    //        }
    //        //ShowGameInfoOverlay();
    //        m_renderNeeded = true;
    //    }

    //    if (m_haveFocus && m_onRenderingEventToken.Value == 0)
    //    {
    //        // App is now "active" so set up the event handler to do game processing and rendering
    //        m_onRenderingEventToken = CompositionTarget::Rendering::add(ref new EventHandler<Object^>(this, &App::OnRendering));
    //    }
    //}
  }

  //--------------------------------------------------------------------------------------

  //void App::SetGameInfoOverlay(GameInfoOverlayState state)
  //{
  // m_gameInfoOverlayState = state;
  // switch (state)
  // {
  // case GameInfoOverlayState::Loading:
  //     m_mainPage->SetGameLoading();
  //     break;

  // case GameInfoOverlayState::GameStats:
  //     m_mainPage->SetGameStats(
  //         m_game->HighScore().levelCompleted + 1,
  //         m_game->HighScore().totalHits,
  //         m_game->HighScore().totalShots
  //         );
  //     break;

  // case GameInfoOverlayState::LevelStart:
  //     m_mainPage->SetLevelStart(
  //         m_game->LevelCompleted() + 1,
  //         m_game->CurrentLevel()->Objective(),
  //         m_game->CurrentLevel()->TimeLimit(),
  //         0
  //         );
  //     break;

  // case GameInfoOverlayState::GameOverCompleted:
  //     m_mainPage->SetGameOver(
  //         true,
  //         m_game->LevelCompleted() + 1,
  //         0,
  //         0,
  //         m_game->HighScore().totalHits
  //         );
  //     break;

  // case GameInfoOverlayState::GameOverExpired:
  //     m_mainPage->SetGameOver(
  //         false,
  //         m_game->LevelCompleted(),
  //         0,
  //         0,
  //         m_game->HighScore().totalHits
  //         );
  //     break;

  // case GameInfoOverlayState::Pause:
  //     m_mainPage->SetPause(
  //         m_game->LevelCompleted() + 1,
  //0,
  //0,
  //0
  //         );
  //     break;
  // }
  //}



  //--------------------------------------------------------------------------------------

  //void App::ResetGame()
  //{
  //m_updateState = UpdateEngineState::WaitingForResources;
  //m_pressResult = PressResultState::PlayLevel;
  //m_controller->Active(false);
  //m_game->LoadGame();
  //SetAction(GameInfoOverlayCommand::PleaseWait);
  //SetGameInfoOverlay(GameInfoOverlayState::LevelStart);
  ////ShowGameInfoOverlay();
  //m_renderNeeded = true;

  //m_game->LoadLevelAsync().then([this]()
  //{
  //    m_game->FinalizeLoadLevel();
  //    m_updateState = UpdateEngineState::ResourcesLoaded;
  //    m_renderNeeded = true;

  //}, task_continuation_context::use_current());
  //}

  //--------------------------------------------------------------------------------------

  void GameMain::InitializeLicense()
  {
#ifdef USE_STORE_SIMULATOR
    m_licenseState = ref new PersistentState();
    m_licenseState->Initialize(ApplicationData::Current->LocalSettings->Values, "CurrentAppSimulator");
    m_isTrial = m_licenseState->LoadBool(":isTrial", true);

    Platform::String^ license;
    if (this->m_isTrial)
    {
      license = "TrialLicense.xml";
    }
    else
    {
      license = "FullLicense.xml";
    }
    task<StorageFile^> fileTask(Package::Current->InstalledLocation->GetFileAsync(license));
    fileTask.then([=](StorageFile^ sourceFile)
    {
      return create_task(CurrentAppSimulator::ReloadSimulatorAsync(sourceFile));
    }).then([this]()
    {
      this->InitializeLicenseCore();
    });
#else
    this->InitializeLicenseCore();
#endif
  }

  //--------------------------------------------------------------------------------------

  void GameMain::InitializeLicenseCore()
  {
#ifdef USE_STORE_SIMULATOR
    this->m_licenseInformation = CurrentAppSimulator::LicenseInformation;
    task<ListingInformation^> listingTask(CurrentAppSimulator::LoadListingInformationAsync());
#else
    m_licenseInformation = CurrentApp::LicenseInformation;
    task<ListingInformation^> listingTask(CurrentApp::LoadListingInformationAsync());
#endif

    this->m_licenseInformation->LicenseChanged += ref new LicenseChangedEventHandler(this, &GameMain::OnLicenseChanged);
    this->m_mainPage->SetProductItems(nullptr, m_licenseInformation);
    this->OnLicenseChanged();

    listingTask.then([=](ListingInformation^ listing)
    {
      this->m_listingInformation = listing;
      this->OnLicenseChanged();
    });
  }

  //--------------------------------------------------------------------------------------

#ifdef USE_STORE_SIMULATOR
  void GameMain::ResetLicense()
  {
    task<StorageFile^> fileTask(Package::Current->InstalledLocation->GetFileAsync("TrialLicense.xml"));
    fileTask.then([=](StorageFile^ sourceFile)
    {
      CurrentAppSimulator::ReloadSimulatorAsync(sourceFile);
    });
  }
#endif

  //--------------------------------------------------------------------------------------

  void GameMain::OnLicenseChanged()
  {
#ifdef USE_STORE_SIMULATOR
    m_isTrial = (m_licenseInformation->IsActive && m_licenseInformation->IsTrial);
    m_licenseState->SaveBool(":isTrial", m_isTrial);
#endif
    m_mainPage->LicenseChanged(m_listingInformation, m_licenseInformation);
    UpdateGameConfig(m_licenseInformation);

    if (m_licenseInformation->IsActive)
    {
      if (m_licenseInformation->IsTrial)
      {
        m_renderer->Hud()->SetLicenseInfo("Trial Version");
      }
      else
      {
        m_renderer->Hud()->SetLicenseInfo("Full Version");
      }
    }
    else
    {
      m_renderer->Hud()->SetLicenseInfo("License Inactive - defaulting to Trial Version");
    }

    m_renderNeeded = true;
  }

  void GameMain::UpdateGameOver()
  {
    m_renderer->RenderGameOver();
  }

  void GameMain::OnKeyDown(
    _In_ CoreWindow^ /* sender */,
    _In_ KeyEventArgs^ args
    )
  {
    Windows::System::VirtualKey Key;
    Key = args->VirtualKey;
    if(Key == Windows::System::VirtualKey::Escape)
    {
      GetStateMachine().process_event(stm::ev::Escape(this));
    };
  }
}


