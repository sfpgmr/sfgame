#pragma once
//#include "singleton.h"

namespace sf {
  ref class GameMain;
  namespace stm {

      // ゲーム中のイベント定義
      namespace ev {

        
        template <typename Tag>
        struct EventBase 
        {
          EventBase(GameMain^ main) : main_(main) {};

          template <typename SrcEvent>
          EventBase(SrcEvent& src) : main_(src.getGameMain())
          {

          }

          GameMain^ getGameMain() const {return main_;};
        private:
          GameMain^ main_;
        };

        struct AppLaunchedT {};
        struct InitCompleteT {};
        struct SelectEditT {};
        struct SelectPlayT {};
        struct StartGameT {};
        struct CompleteT{};
        struct PlayerIsGoneT{};
        struct PlayerLeftZeroT{};
        struct EscapeT {};
        struct ShowAppMenuT {};
        struct HideAppMenuT {};
        struct ExitT {};
        struct TimeOverT {};
        struct UpdateT {};
        struct ReturnMenuT {};
        struct PlayContinueT {};

        typedef EventBase<AppLaunchedT> AppLaunched;
        typedef EventBase<InitCompleteT> InitComplete;
        typedef EventBase<SelectEditT> SelectEdit;
        typedef EventBase<SelectPlayT> SelectPlay;
        typedef EventBase<StartGameT> StartGame;
        typedef EventBase<CompleteT> Complete;
        typedef EventBase<PlayerIsGoneT> PlayerIsGone;
        typedef EventBase<PlayerLeftZeroT> PlayerLeftZero;
        typedef EventBase<EscapeT> Escape;
        typedef EventBase<ShowAppMenuT> ShowAppMenu;
        typedef EventBase<HideAppMenuT> HideAppMenu;
        typedef EventBase<ExitT> Exit;
        typedef EventBase<TimeOverT> TimeOver;
        typedef EventBase<UpdateT> Update;
        typedef EventBase<ReturnMenuT> ReturnMenu;
        typedef EventBase<PlayContinueT> PlayContinue;

      }

      struct game_stm 
      {
        game_stm();
        ~game_stm();
        template <typename Event>  inline void process_event(Event& ev);
        void start();
        void stop();
        struct impl;
      private:
        std::unique_ptr<impl> impl_;
      };

  }
}

