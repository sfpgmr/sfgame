﻿//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#pragma once

// App:
// This class is the main App class required for XAML Metro style apps.
// It it called on Launch activation and maintains the overall state of the game.
// The App class drives and maintains a state machine for the game.  It can be in
// one of seven major stated defined by the UpdateEngineState enum class.
// These are:
//     WaitingForResources - the game has requested the game object or the renderer object
//         to load resources asynchronously.
//     ResourcesLoaded - the asynchronous loading of resources has been completed.  This
//         is a transient state.
//     WaitingForPress - the game is waiting for the player to indicate they are ready to proceed.
//         There are three possible actions from this state.  This is controlled by m_pressResult.
//         The possible outcomes are:
//             LoadGame - The player is ready to start a new game and has acknowledged the status
//                 information provided about the previous state.
//             PlayLevel - The player is ready to play the next level.  The level has already been
//                 loaded so active game play will start.
//             ContinueLevel - The player is ready to continue playing the current level.  Part of the
//                 current level has already been played.
//     Dynamics - the game is active play mode.
//     Snapped - the game is currently in the snapped position on the m_screenInfo.  It is not possible to
//         play the game in snapped mode.
//     Suspended - the game was suspended by PLM.
//     Deactivated - the game has lost focus.
//
// App creates and maintains references to three major objects used for the game:
//     MoveLookController (m_controller) - this object handles all the game specific user input and
//         aggregates touch, mouse/keyboard and Xbox controller input into a unified input control.
//     GameMain (m_game) - this object handles all the game specific logic and game dynamics.
//     GameRenderer (m_renderer) - This object handles all the graphics rendering for the game.
//
// App registers for all the necessary Metro style events to maintain and control all state transitions.
// The App class dynamically registers and unregisters for the CompositionTarget::Rendering event based on whether
// rendering is needed or not.  This event will occur on each vertical retrace to give a steady set of frames for
// smooth scene rendering.

#if defined(_DEBUG)
#define DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION 1
#endif

#define USE_STORE_SIMULATOR 1

#include "gamemain.h"
#ifdef USE_STORE_SIMULATOR
#include "PersistentState.h"
#endif
#include "App.g.h"

namespace sf
{
    //private enum class UpdateEngineState
    //{
    //    WaitingForResources,
    //    ResourcesLoaded,
    //    WaitingForPress,
    //    Dynamics,
    //    Snapped,
    //    Suspended,
    //    Deactivated,
    //};

    //private enum class PressResultState
    //{
    //    LoadGame,
    //    PlayLevel,
    //    ContinueLevel,
    //};

    //private enum class GameInfoOverlayState
    //{
    //    Loading,
    //    GameStats,
    //    GameOverExpired,
    //    GameOverCompleted,
    //    LevelStart,
    //    Pause,
    //    Snapped,
    //};

    [Windows::Foundation::Metadata::WebHostHidden]
    public ref class App sealed
    {
    public:
        App();

        virtual void OnLaunched(
            _In_ Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ args
            ) override;

        //void PauseRequested() { if (m_updateState == UpdateEngineState::Dynamics) m_pauseRequested = true; };
        //void PressComplete()  { if (m_updateState == UpdateEngineState::WaitingForPress) m_pressComplete = true; };
        //void ResetGame();
        //void SetBackground(unsigned int background);
        //void CycleBackground();

 //       void SetGameInfoOverlay(GameInfoOverlayState state);
//        void SetAction (GameInfoOverlayCommand command);
        //void //ShowGameInfoOverlay();
        //void HideGameInfoOverlay();
        //void SetSnapped();
        //void HideSnapped();

        //UpdateEngineState                                   m_updateState;
        //UpdateEngineState                                   m_updateStateNext;
        //PressResultState                                    m_pressResult;
        //GameInfoOverlayState                                m_gameInfoOverlayState;

    private:
      GameMain^ m_gameMain;

    };
}


