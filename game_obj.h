#pragma once
#include "game_base.h"
#include "game_obj_base.h"
namespace sf {
class game_obj : public game_obj_base
{
public:
  game_obj(game_base& game); 
  virtual void move(float delta_time);
  virtual ~game_obj(){};
private:
};
}

