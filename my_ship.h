#pragma once
#include "game_obj.h"
namespace sf {
struct my_ship : public game_obj
{
public:
  my_ship(game_base& game);
  virtual ~my_ship();
  virtual void move(float delta_time){};
};
}

