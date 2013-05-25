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

//--------------------------------------------------------------------------------------

enum class GameState
{
    Waiting,
    Active,
    LevelComplete,
    TimeExpired,
    GameComplete,
};

typedef struct
{
    Platform::String^ tag;
    int totalHits;
    int totalShots;
    int levelCompleted;
} HighScoreEntry;

typedef std::vector<HighScoreEntry> HighScoreEntries;

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

    void LoadGame();
    concurrency::task<void> LoadLevelAsync();
    void FinalizeLoadLevel();
    void StartLevel();
    void PauseGame();
    void ContinueGame();
    GameState RunGame();
    void SetCurrentLevelToSavedState();

    void OnSuspending();
    void OnResuming();

    bool IsActivePlay()                         { return m_timer->Active(); }
    bool IsTrial()                              { return m_gameConfig.isTrial; }
    int LevelCompleted()                        { return m_currentLevel; };
    bool GameActive()                           { return m_gameActive; };
    bool LevelActive()                          { return m_levelActive; };
    HighScoreEntry HighScore()                  { return m_topScore; };
    Level^ CurrentLevel()                       { return m_level[m_currentLevel]; };
    Camera^ GameCamera()                        { return m_camera; };
	  std::vector<GameObject^> RenderObjects()    { return m_renderObjects; };
	  sf::game_base& game() {return m_game;} 
    float processTime(){return m_processTime;}

    void UpdateGameConfig(Windows::ApplicationModel::Store::LicenseInformation^ licenseInformation);
    void SetBackground(uint32 background);
    void CycleBackground();



private:
    void LoadState();
    void SaveState();
    void SaveHighScore();
    void LoadHighScore();
    void InitializeAmmo();
    void UpdateDynamics();
    void InitializeGameConfig();

    MoveLookController^                         m_controller;
    GameRenderer^                               m_renderer;
  	Camera^                                     m_camera;

    Audio^                                      m_audioController;


    HighScoreEntry                              m_topScore;
    PersistentState^                            m_savedState;

    GameTimer^                                  m_timer;
    bool                                        m_gameActive;
    bool                                        m_levelActive;
    std::vector<Level^>                         m_level;
    uint32                                      m_levelCount;
    uint32                                      m_currentLevel;

    GameConfig                                  m_gameConfig;
    uint32                                      m_activeBackground;
	  sf::scroll_game		m_game;
    float m_processTime;

    std::vector<GameObject^>                    m_objects;           // List of all objects to be included in intersection calculations.
    std::vector<GameObject^>                    m_renderObjects;     // List of all objects to be rendered.


};

