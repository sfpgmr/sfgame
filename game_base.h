#pragma once
#include <Box2D.h>

namespace sf {

  struct game_base {
    game_base(){};
    virtual ~game_base(){};
    virtual b2World& world() = 0;
    virtual float32 scroll_offset() = 0;
    virtual void step() = 0;
  };
}