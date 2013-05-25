#pragma once
#include "game_base.h"
namespace sf {
class game_obj_base
{
public:
  game_obj_base(game_base& game) : game_base_(game),used_(false){};
  virtual ~game_obj_base(){};
  virtual void move(float delta_time) = 0;
  virtual bool is_clip(){return clip_;}
  bool is_used(){return used_;}
  void used(bool v){used_ = v;}
protected:
  game_base& game_base_;
  b2Body* body_;
  bool clip_;
  bool used_;
};
}

