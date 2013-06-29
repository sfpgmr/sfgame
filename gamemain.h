//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

// GameMain:
// This is the main game class.  It controls game play logic and game state.
// Some of the key object classes used by GameMain are:
//     MoveLookController - for handling all input to control player movement, aiming,
//         and firing.
//     GameRenderer - for handling all graphics presentation.
//     Camera - for handling view projections.
//     Audio - for handling sound output.
// This class maintains several lists of objects:
//     m_ammo <Sphere> - is the list of the balls used to throw at targets.  GameMain
//         cycles through the list in a LRU fashion each time a ball is thrown by the player.
//     m_objects <GameObject> - is the list of all objects in the scene that participate in
//         game physics.  This includes m_player <Sphere> to represent the player in the scene.
//         The player object (m_player) is not visible in the scene so it is not rendered.
//     m_renderObjects <GameObject> - is the list of all objects in the scene that may be
//         rendered.  It includes both the m_ammo list, most of the m_objects list excluding m_player
//         object and the objects representing the bounding world.

#include "MainPage.xaml.h"
#include "MenuPage.xaml.h"
#include "GameConstants.h"
#include "Audio.h"
#include "Level.h"
#include "GameObject.h"
#include "GameTimer.h"
#include "MoveLookController.h"
#include "PersistentState.h"
#include "GameRenderer.h"
#include "sfmf.h"
#include "Camera.h"
#include "game.h"
#include "stm_main.h"

//--------------------------------------------------------------------------------------
namespace sf {
//enum class GameState
//{
//    Waiting,
//    Active,
//    LevelComplete,
//    GameComplete
//};

typedef struct
{
  Platform::String^ Name;
  int Score;
} HighScoreEntry;

#define HIGHSCORE_SIZE 10
typedef std::array<HighScoreEntry,HIGHSCORE_SIZE> HighScoreEntries;

typedef struct
{
  bool isTrial;
  bool autoFire;
  bool backgroundAvailable[GameConstants::MaxBackgroundTextures];
} GameConfig;

//--------------------------------------------------------------------------------------

ref class GameRenderer;

ref class GameMain 
{
internal:
  GameMain();

  void Initialize(
    _In_ MoveLookController^ controller,
    _In_ GameRenderer^ renderer
    );


  void OnSuspending();
  void OnResuming();

  void LoadGame();
  concurrency::task<void> LoadLevelAsync();
  void FinalizeLoadLevel();
  void StartLevel();
  void RestartLevel();
  void PauseGame();
  void ContinueGame();
  void RunGame();
  void UpdateGameOver();
  void SetCurrentLevelToSavedState();
  void ShowMenu();
  bool IsActivePlay()                         { return m_timer->Active(); }
  bool IsTrial()                              { return m_gameConfig.isTrial; }
  int  LevelCompleted()                       { return m_currentLevel; };
  bool GameActive()                           { return m_gameActive; };
  bool LevelActive()                          { return m_levelActive; };
  property int score;
  property HighScoreEntries& highScoreEntries
  {
    HighScoreEntries& get() {return m_highScoreEntries;}
  }
  Level^ CurrentLevel()                       { return m_level[m_currentLevel]; };
  Camera^ GameCamera()                        { return m_camera; };
  std::vector<GameObject^> RenderObjects()    { return m_renderObjects; };
  game_base& game() {return m_game;} 
  float processTime(){return m_processTime;}

  void UpdateGameConfig(Windows::ApplicationModel::Store::LicenseInformation^ licenseInformation);

  stm::game_stm& GetStateMachine() {return game_stm_;};

  property Windows::ApplicationModel::Activation::ApplicationExecutionState previousExecutionState;
  property int32 playerLeft;

#ifdef USE_STORE_SIMULATOR
  void ResetLicense();
#endif
  void ActionOnLaunched();

private:

  ~GameMain()
  {
    Windows::UI::Xaml::Media::CompositionTarget::Rendering::remove(m_onRenderingEventToken);
  }

  void OnSuspending(
    _In_ Platform::Object^ sender,
    _In_ Windows::ApplicationModel::SuspendingEventArgs^ args
    );

  void OnResuming(
    _In_ Platform::Object^ sender,
    _In_ Platform::Object^ args
    );

  void UpdateViewState();

  void OnWindowActivationChanged(
    _In_ Platform::Object^ sender,
    _In_ Windows::UI::Core::WindowActivatedEventArgs^ args
    );

  void OnWindowSizeChanged(
    _In_ Windows::UI::Core::CoreWindow^ sender,
    _In_ Windows::UI::Core::WindowSizeChangedEventArgs^ args
    );

  void OnLogicalDpiChanged(
    _In_ Platform::Object^ sender
    );

  void OnDisplayContentsInvalidated(
    _In_ Platform::Object^ sender
    );

  void OnVisibilityChanged(
    _In_ Windows::UI::Core::CoreWindow^ sender,
    _In_ Windows::UI::Core::VisibilityChangedEventArgs^ args
    );

  void OnRendering(
    _In_ Object^ sender,
    _In_ Object^ args
    );

  void OnLicenseChanged();
  void InitializeLicense();
  void InitializeLicenseCore();

  void OnDeviceLost();
  void OnDeviceReset();
  void OnKeyDown(
    _In_ Windows::UI::Core::CoreWindow^  sender,
    _In_ Windows::UI::Core::KeyEventArgs^ args
    );
  void Update();

  void LoadState();
  void SaveState();
  void SaveHighScore();
  void LoadHighScore();
  void UpdateDynamics();
  void InitializeGameConfig();

  MainPage^                                   m_mainPage;
  MoveLookController^                         m_controller;
  GameRenderer^                               m_renderer;
  Camera^                                     m_camera;

  Audio^                                      m_audioController;


  HighScoreEntries                            m_highScoreEntries;
  PersistentState^                            m_savedState;

  GameTimer^                                  m_timer;
  bool                                        m_gameActive;
  bool                                        m_levelActive;
  std::vector<Level^>                         m_level;
  uint32                                      m_levelCount;
  uint32                                      m_currentLevel;
  
  GameConfig                                  m_gameConfig;
  uint32                                      m_activeBackground;
  scroll_game		m_game;
  float m_processTime;

  std::vector<GameObject^>                    m_objects;           // List of all objects to be included in intersection calculations.
  std::vector<GameObject^>                    m_renderObjects;     // List of all objects to be rendered.


  Windows::Foundation::EventRegistrationToken         m_onRenderingEventToken;
  bool                                                m_pauseRequested;
  bool                                                m_pressComplete;
  bool                                                m_renderNeeded;
  bool                                                m_haveFocus;
  bool                                                m_visible;
  Windows::ApplicationModel::Store::LicenseInformation^ m_licenseInformation;
  Windows::ApplicationModel::Store::ListingInformation^ m_listingInformation;
#ifdef USE_STORE_SIMULATOR
  PersistentState^                                    m_licenseState;
  bool                                                m_isTrial;
#endif

  stm::game_stm game_stm_;
};
}

