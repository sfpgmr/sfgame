#include "pch.h"
//#include "App.xaml.h"
#include "stm_main.h"
//#include "MainPage.xaml.h"
#include "gamemain.h"

#include "stm_action.h"

using namespace Concurrency;

namespace sf {
  namespace stm {

    template <typename EventClass,int Resolution>
    task<void> create_timer_task(int ms,GameMain^ main,cancellation_token& token)
    {
      return create_task([=]()
      {
        int count = ms;
        while(count > 0 && !token.is_canceled()){
          if(count < Resolution)
          {
            wait(count);
            break;
          } else {
            wait(Resolution);
            count -= Resolution;
          }
        }
      },token).then([=](){
        main->GetStateMachine().process_event(EventClass(main));
      },Concurrency::task_continuation_context::use_current());
    }


    namespace msmf = boost::msm::front;
    template<typename T1,typename T2,typename T3,typename T4,typename T5>  struct R 
      : public msmf::Row<T1,T2,T3,T4,T5>{};


    struct NotInit : msmf::state<>{};
    struct Menu : msmf::state<> 
    {
      template <class Event, class Fsm> 
      void on_entry(Event const& ev, Fsm& fsm) 
      {
        std::function<void ()> null_func([]()->void{});

        GameMain^ m = ev.getGameMain();
        // メインメニューのセットアップ
        Data::MainMenuDataSource^ data = Data::MainMenuDataSource::Instance;
        data->AllGroups->Clear();

        auto group1 = ref new Data::MainMenuDataGroup(L"GamePlayGroup",
          L"ゲームプレイ",
          L"ゲームプレイ",
          L"",
          L"",null_func);

        group1->Items->Append(ref new Data::MainMenuDataItem("GamePlayGroup00",
          L"新規ゲーム",
          L"新規ゲーム",
          L"",L"",L"",group1,std::function<void()>(
          [&fsm,m]()->void{
            fsm.process_event(ev::SelectPlay(m));
         }
        )));
        group1->Items->Append(ref new Data::MainMenuDataItem("GamePlayGroup01",
          L"ゲームロード",
          L"ゲームロード",
          L"",
          L"",
          L"",
          group1,null_func));

        data->AllGroups->Append(group1);

        auto group11 = ref new Data::MainMenuDataGroup(L"ハイスコア",
          L"ハイスコア情報",
          L"ハイスコア情報",
          L"",
          L"",null_func);

        group11->Items->Append(ref new Data::MainMenuDataItem("GameEditGroup03",
          L"ハイスコア表示",
          L"ハイスコア表示",
          L"",
          L"",
          L"",
          group11,null_func));

        data->AllGroups->Append(group11);

        auto group2 = ref new Data::MainMenuDataGroup(L"GameEditGroup",
          L"ゲームエディット",
          L"ゲームエディット",
          L"",
          L"",null_func);
        group2->Items->Append(ref new Data::MainMenuDataItem("GameEditGroup01",
          L"敵動作エディット",
          L"敵動作エディット",
          L"",
          L"",
          L"",
          group2,null_func));

        group2->Items->Append(ref new Data::MainMenuDataItem("GameEditGroup00",
          L"配置エディット",
          L"配置エディット",
          L"",
          L"",
          L"",
          group2,null_func));

        data->AllGroups->Append(group2);

        m->ShowMenu();
      }

    };
    struct Play_ : msmf::state_machine_def<Play_> 
    {
      struct Init : msmf::state<> 
      {
        template <class Event, class Fsm> 
        void on_entry(Event const& , Fsm& ) 
        {
          std::cout <<"entering: Empty" << std::endl;
        }

      };
      struct Waiting : msmf::state<> {};

      struct Active_ : msmf::state_machine_def<Active_>
      {
        // 状態定義
        template <class Event, class Fsm> 
        void on_entry(Event const& ev, Fsm& ) 
        {
          GameMain^ main(ev.getGameMain());
          main->StartLevel();
        }
        struct Init : msmf::state<>{};
        struct Playing : msmf::state<>{};
        struct Pause : msmf::state<>{};
        struct Restart : msmf::state<>{};
        struct CheckRestart : msmf::state<>{};
        struct Exit : public msmf::exit_pseudo_state<ev::Exit>{};
       // struct ReturnMenu : public msmf::exit_pseudo_state<ev::ReturnMenu>{};

        // アクション
        // ガード条件
        struct transition_table : boost::mpl::vector<
          //現在状態      ,イベント          ,次の状態     ,アクション , ガード 
          R <Playing      ,ev::Escape         ,Pause        ,PauseAction ,msmf::none   > ,
          R <Playing      ,ev::ShowAppMenu    ,Pause        ,PauseAction ,msmf::none   > ,
          R <Playing      ,ev::Update         ,msmf::none   ,PlayAction ,msmf::none   > ,
          R <Playing      ,ev::PlayerIsGone   ,msmf::none   ,PlayerIsGoneAction,msmf::none   > ,
          R <Playing      ,ev::PlayerIsGone   ,Exit     ,msmf::none ,CheckPlayerLeft  > ,
          R <Pause        ,ev::Update         ,msmf::none   ,msmf::none ,msmf::none   > ,
          R <Pause        ,ev::Escape         ,Playing      ,ContinueAction,msmf::none   > ,
          R <Pause        ,ev::HideAppMenu    ,Playing      ,ContinueAction,msmf::none   > 
        >{};
        typedef Active_::Playing initial_state;
      };
      typedef boost::msm::back::state_machine<Active_> Active;
      struct LevelComplete : msmf::state<>{};
      //        struct LevelComplete : msmf::state<>{};
      struct GameComplete : msmf::state<>{};
      struct GameOver : msmf::state<>{
        template <class Event, class Fsm> 
        void on_entry(Event const& ev, Fsm& ) 
        {
          GameMain^ g = ev.getGameMain();
          create_timer_task<ev::TimeOver,100>(1000 * 3,g,token_.get_token());
          //timer_task_ = Concurrency::create_task(
          //  [=]() -> void {
          //    Concurrency::wait(1000 * 3);
          //  }
          //  ).then([=]{
          //    g->GetStateMachine().process_event(ev::TimeOver(g));
          //  },Concurrency::task_continuation_context::use_current()
          //  );
        }
        void cancel(){token_.cancel();}
      private:
          cancellation_token_source token_;
//        Concurrency::task<void> timer_task_;
      };

      struct PlayExit : public msmf::exit_pseudo_state<ev::Exit>{};


      // 状態遷移テーブル
      struct transition_table : boost::mpl::vector
        //現在状態      ,イベント           , 次の状態    , アクション   , ガード 
        < 
        R <Active::exit_pt<Active_::Exit>,ev::Exit, GameOver,msmf::none, msmf::none>,
        R <GameOver       ,ev::Escape         ,PlayExit      ,CancelTimerAction, msmf::none > ,
        R <GameOver       ,ev::TimeOver       ,PlayExit      ,msmf::none, msmf::none > ,
        R <GameOver       ,ev::Update         ,msmf::none   ,GameOverUpdateAction, msmf::none> 
        >
      {};
      typedef Active initial_state;
    };

    typedef boost::msm::back::state_machine< Play_ > Play;

    struct Edit : msmf::state<> {};

    struct game_main_ : msmf::state_machine_def<game_main_>
    {
    public:
      game_main_(){};
      virtual ~game_main_(){};
      // 状態遷移テーブル
      struct transition_table : boost::mpl::vector<
        // 現在状態   ,イベント           ,次の状態   , アクション , ガード 
        R <NotInit   ,ev::AppLaunched    ,msmf::none  ,AppLaunchedAction,msmf::none >,
        R <NotInit   ,ev::InitComplete   ,Menu        ,msmf::none  ,msmf::none >,
        R <Menu      ,ev::SelectEdit     ,Edit        ,msmf::none  ,msmf::none  > ,
        R <Menu      ,ev::Update         ,msmf::none  ,UpdateAction ,msmf::none  > ,
        R <Menu      ,ev::SelectPlay     ,Play        ,msmf::none   ,msmf::none  > ,
        R <Play::exit_pt<Play_::PlayExit> ,ev::Exit,Menu ,msmf::none   ,msmf::none >
        >
      {};

      // イベントが見つからなかった時に呼ばれる
      template <class FSM,class Event>
      void no_transition(Event const& e, FSM&,int state)
      {
        //DOUT(boost::wformat(L"**** no transition from state %s ****") % typeid(e).name());
      }

      typedef NotInit initial_state;
    };

    typedef boost::msm::back::state_machine< game_main_ > game_main;

    struct game_stm::impl 
    {
      impl(){}
      ~impl(){}
      template <typename Event> inline void process_event(Event& ev)
      {
        main_.process_event(ev);
      }
      inline void start(){main_.start();}
      inline void stop(){main_.stop();}
    private:
      game_main main_;
    };

    game_stm::game_stm() : impl_(new impl())
    {

    }

    game_stm::~game_stm()
    {
      impl_.reset();
    }

    template <typename Event> void game_stm::process_event(Event& ev)
    {
      impl_->process_event(ev);
    };

    void game_stm::start()
    {
      impl_->start();
    }

    void game_stm::stop()
    {
      impl_->stop();
    }

    template void game_stm::process_event<ev::InitComplete>(ev::InitComplete& ev);
    template void game_stm::process_event<ev::SelectEdit>(ev::SelectEdit& ev);
    template void game_stm::process_event<ev::SelectPlay>(ev::SelectPlay& ev);
    template void game_stm::process_event<ev::StartGame>(ev::StartGame& ev);
    template void game_stm::process_event<ev::Complete>(ev::Complete& ev);
    template void game_stm::process_event<ev::PlayerIsGone>(ev::PlayerIsGone& ev);
    template void game_stm::process_event<ev::PlayerLeftZero>(ev::PlayerLeftZero& ev);
    template void game_stm::process_event<ev::Escape>(ev::Escape& ev);
    template void game_stm::process_event<ev::Exit>(ev::Exit& ev);
    template void game_stm::process_event<ev::TimeOver>(ev::TimeOver& ev);
    template void game_stm::process_event<ev::Update>(ev::Update& ev);
    template void game_stm::process_event<ev::AppLaunched>(ev::AppLaunched& ev);
    template void game_stm::process_event<ev::ReturnMenu>(ev::ReturnMenu& ev);
    template void game_stm::process_event<ev::ShowAppMenu>(ev::ShowAppMenu& ev);
    template void game_stm::process_event<ev::HideAppMenu>(ev::HideAppMenu& ev);

  }
}
