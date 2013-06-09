#pragma once
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>

namespace sf {
  namespace stm {
    namespace msmf = boost::msm::front;
    namespace menu {
      struct menu : msmf::state<> {};
      struct game : msmf::state<> {};

      namespace ev {
        struct game {};
      }

      struct main_menu_ : msmf::state_machine_def<main_menu_>
      {
      public:
        main_menu_(){};
        virtual ~main_menu_(){};
        // 状態遷移テーブル
        struct transition_table : boost::mpl::vector
          //            現在状態      ,イベント           , 次の状態      , アクション               , ガード 
          < _row       <game        ,ev::game          ,game          >
          // a_row       <msmf::interrupt_state    ,ev::Close         ,Closed         ,&Player_::shutdown>
          >
        {};
        typedef game initial_state;

      };
      typedef boost::msm::back::state_machine< main_menu_ > main_menu;

    }
  }
}

