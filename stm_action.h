#pragma once
namespace sf 
{
  namespace stm {
    
    struct PlayAction 
    {
      template <class EVT,class FSM,class SourceState,class TargetState>
      void operator()(EVT const & ev ,FSM& ,SourceState& ,TargetState& )
      {
        DOUT(L"PlayAction : Default Action");
        //GameMain^ const main = ev.getGameMain();
        //main->RunGame();
        //OutputDebugStringW(L"PlayAction");
      }

      template <class FSM,class SourceState,class TargetState>
      void operator()(ev::Update const & ev ,FSM& ,SourceState& ,TargetState& )
      {
        GameMain^ const main = ev.getGameMain();
        main->RunGame();
      }
    };

    struct PauseAction
    {
      template <class FSM,class SourceState,class TargetState>
      void operator()(ev::Escape const & ev ,FSM& ,SourceState& ,TargetState& )
      {
        ev.getGameMain()->PauseGame();
      }
    };

    struct ContinueAction
    {
      template <class FSM,class SourceState,class TargetState>
      void operator()(ev::Escape const & ev ,FSM& ,SourceState& ,TargetState& )
      {
        ev.getGameMain()->ContinueGame();
      }
    };

    struct PauseUpdate 
    {
      template <class EVT,class FSM,class SourceState,class TargetState>
      void operator()(EVT const & ev ,FSM& ,SourceState& ,TargetState& )
      {
        GameMain^ const main = ev.getGameMain();
        //if(drawed)
      }
      static void reset() {drawed = true;}
    private:
      static bool drawed;
    };

    
    struct AppLaunchedAction {
      template <class EVT,class FSM,class SourceState,class TargetState>
      void operator()(EVT & ev ,FSM& ,SourceState& ,TargetState& )
      {
        GameMain^ const main = ev.getGameMain();
        main->ActionOnLaunched();
      }

    };

    struct PlayStartAction {
      template <class EVT,class FSM,class SourceState,class TargetState>
      void operator()(EVT & ev ,FSM& ,SourceState& ,TargetState& )
      {
        GameMain^ const main = ev.getGameMain();
        main->StartLevel();
       
      }

    };

    struct UpdateAction {

      template <class EVT,class FSM,class SourceState,class TargetState>
      void operator()(EVT & ev ,FSM& ,SourceState& ,TargetState& )
      {
        GameMain^ const main = ev.getGameMain();
      }


    };

    struct PlayerIsGoneAction {
      template <class EVT,class FSM,class SourceState,class TargetState>
      void operator()(EVT const & ev ,FSM& fsm,SourceState& ,TargetState& )
      {
        GameMain^ const main = ev.getGameMain();
        main->playerLeft--;
        //main->StartLevel();
      }

    };

    struct CheckPlayerLeft {
      template <class Event, class Fsm, class SourceState, class TargetState>
      bool operator()(Event const& ev, Fsm& fsm, SourceState&, TargetState&) const 
      {
        GameMain^ const main = ev.getGameMain();
        if(main->playerLeft == 1)
        {
          main->playerLeft = 0;
          return true;
        }
        return false;
      }
    };

    struct GameOverUpdateAction {
      template <class EVT,class FSM,class SourceState,class TargetState>
      void operator()(EVT const & ev ,FSM& fsm,SourceState& ,TargetState& )
      {
        GameMain^ const main = ev.getGameMain();
        main->UpdateGameOver();
      }
    };

    struct CancelTimerAction {
      template <class EVT,class FSM,class SourceState,class TargetState>
      void operator()(EVT const & ,FSM& fsm,SourceState& s,TargetState& )
      {
        s.cancel();
      }
    };
  }
}