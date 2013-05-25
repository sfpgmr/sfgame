#pragma once

namespace sf {
  class game_obj_base;
  typedef std::array<std::unique_ptr<game_obj_base>,100> geme_objs_t;

  struct game_base {
    game_base(){};
    virtual ~game_base(){};
    virtual b2World& world() = 0;
    virtual float32 scroll_offset() = 0;
    virtual void step(float32 delta_time) = 0;
    virtual void screen_aabb(float width,float height,float scale) = 0;
    virtual const b2AABB& screen_aabb() const = 0;
    virtual const b2AABB& screen_actual_aabb() const = 0;
    virtual geme_objs_t& game_objs() = 0;

  };
}