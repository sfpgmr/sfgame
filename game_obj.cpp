#include "pch.h"
#include "game_obj.h"

namespace sf {
  game_obj::game_obj(game_base& game) : game_obj_base(game)
  {
    used(true);
    clip_ = false;
    b2World& world(game_base_.world());
    b2BodyDef body_def;
    body_def.type = b2_dynamicBody;
    body_def.position.Set(game_base_.scroll_offset() + (float32)(rand() & 0x7fff) / 32767.0f * 20.0f - 2.5f + 120.0f  ,90.0f);
    body_ = world.CreateBody(&body_def);
    float32 w = (float32)(rand() & 0x7fff) / 32767.0f * 13.5f + 0.5f;
    b2FixtureDef fixture_def;
    fixture_def.density = 1.0f;
    fixture_def.friction = 0.5f;

    body_->SetUserData(this);

    if(!(rand() & 0x1)){
      b2PolygonShape dynamic_box;
      dynamic_box.SetAsBox(w,w);
      fixture_def.shape = &dynamic_box;
      body_->CreateFixture(&fixture_def);
    } else {
      b2CircleShape dynamic_circle;
      dynamic_circle.m_radius = w;
      dynamic_circle.m_p.SetZero();
      fixture_def.shape = &dynamic_circle;
      body_->CreateFixture(&fixture_def);
    }
  }

  void game_obj::move(float delta_time)
  {
    // ‰æ–Ê‹«ŠE‚æ‚èÁŽ¸‚µ‚½‚çÁ‚·
    b2Fixture* fix = body_->GetFixtureList();
    b2AABB aabb;
    aabb.lowerBound.SetZero();
    aabb.upperBound.SetZero();

    while(fix)
    {
      b2Shape* shape = fix->GetShape();
      if(shape)
      {
        b2AABB aabb_t;
        shape->ComputeAABB(&aabb_t,body_->GetTransform(),0);
        aabb.Combine(aabb_t);
      }
      fix = fix->GetNext();
    }

    if(!clip_)
    {
      if(b2TestOverlap(game_base_.screen_actual_aabb(),aabb))
      {
        clip_ = true;
      }
    } else {
      if(!b2TestOverlap(game_base_.screen_actual_aabb(),aabb))
      {
        used(false);
        game_base_.world().DestroyBody(body_);
        body_ = nullptr;
      }
    }
  }
}

