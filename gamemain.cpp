//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "ConstantBuffers.h"
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
using namespace concurrency;
using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Windows::ApplicationModel::Store;
using namespace Windows::Storage;
using namespace Windows::UI::Core;

//----------------------------------------------------------------------

GameMain::GameMain():
    m_gameActive(false),
    m_levelActive(false),
    m_levelCount(0),
    m_currentLevel(0),
    m_activeBackground(0),m_processTime(0.0f)
{
    m_topScore.totalHits = 0;
    m_topScore.totalShots = 0;
    m_topScore.levelCompleted = 0;
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
    InitializeAmmo();
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
    m_timer->Reset();
    m_timer->Start();
    if (m_currentLevel == 0)
    {
        m_gameActive = true;
    }
    m_levelActive = true;
    m_controller->Active(true);
}

//----------------------------------------------------------------------

void GameMain::PauseGame()
{
    m_timer->Stop();
    SaveState();
}

//----------------------------------------------------------------------

void GameMain::ContinueGame()
{
    m_timer->Start();
    m_controller->Active(true);
}

//----------------------------------------------------------------------

GameState GameMain::RunGame()
{
    // This method is called to execute a single time interval for active game play.
    // It returns the resulting state of game play after the interval has been executed.

    m_timer->Update();

        // Time has not expired, so run one frame of game play.

        UpdateDynamics();

        // Update the Camera with the player position updates from the dynamics calculations.

    return GameState::Active;
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
    m_savedState->SaveInt32(":HighScore:LevelCompleted", m_topScore.levelCompleted);
    m_savedState->SaveInt32(":HighScore:TotalShots", m_topScore.totalShots);
    m_savedState->SaveInt32(":HighScore:TotalHits", m_topScore.totalHits);
}

//----------------------------------------------------------------------

void GameMain::LoadHighScore()
{
    m_topScore.levelCompleted = m_savedState->LoadInt32(":HighScore:LevelCompleted", 0);
    m_topScore.totalShots = m_savedState->LoadInt32(":HighScore:TotalShots", 0);
    m_topScore.totalHits = m_savedState->LoadInt32(":HighScore:TotalHits", 0);
}

//----------------------------------------------------------------------

void GameMain::InitializeAmmo()
{
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

//--------------------------------------------------------------------------------------

void GameMain::SetBackground(uint32 background)
{
    if (background < GameConstants::MaxBackgroundTextures)
    {
        if (m_gameConfig.backgroundAvailable[background])
        {
            m_activeBackground = background;
            m_renderer->SetBackground(background);
        }
    }
}

//--------------------------------------------------------------------------------------

void GameMain::CycleBackground()
{
    // There may be gaps in the background textures list that are available for use because the
    // user has not purchased all them in order or at all.
    // Cycle through the list looking for the next one that is valid.  This may require wrapping
    // back to the beginning.

    unsigned int newBackground = m_activeBackground;

    while (newBackground < GameConstants::MaxBackgroundTextures)
    {
        newBackground++;
        if (newBackground >= GameConstants::MaxBackgroundTextures)
        {
            newBackground = 0;
            break;
        }
        else if (m_gameConfig.backgroundAvailable[newBackground])
        {
            break;
        }
    }

    if (newBackground != m_activeBackground)
    {
        m_activeBackground = newBackground;
        m_renderer->SetBackground(newBackground);
    }
}
//--------------------------------------------------------------------------------------

