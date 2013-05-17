#pragma once
#include "game_base.h"

namespace sf {
struct scroll_test : game_base
{
public:
  scroll_test();
  virtual ~scroll_test();
  virtual void step();
  virtual float32 scroll_offset(){return scroll_offset_;}
  virtual b2World& world(){return world_;};

private:
  b2World world_;
  b2Body * ground_body_;
  b2Body * body_;
  b2Fixture * body_fixture_;
  b2Fixture * ground_fixture_;
  int32 velocity_iterations_;
  int32 position_iterations_;
  float32 time_step_;
  float32 scroll_offset_;
  ULONGLONG tick_;
};

}

