#pragma once
#include "game_base.h"
#include "game_obj_base.h"

namespace sf {
struct scroll_game : game_base
{
public:
  scroll_game();
  virtual ~scroll_game();
  virtual void init();
  virtual void step(float32 delta_time);
  virtual float32 scroll_offset(){return scroll_offset_;}
  virtual b2World& world(){return *world_;};
  virtual void screen_aabb(float width,float height,float scale);
  virtual const b2AABB& screen_aabb() const 
  {
    return screen_aabb_;
  }
  virtual const b2AABB& screen_actual_aabb() const {return screen_actual_aabb_;}
  virtual game_objs_t& game_objs() {return objs_;}

private:
  
  std::unique_ptr<b2World> world_;
  b2Body * ground_body_;
  b2Body * body_;
  b2Fixture * body_fixture_;
  b2Fixture * ground_fixture_;
  int32 velocity_iterations_;
  int32 position_iterations_;
  float32 time_step_;
  float32 scroll_offset_;
  ULONGLONG tick_;
  b2AABB screen_aabb_;
  b2AABB screen_actual_aabb_;
  game_objs_t objs_;
};

}

