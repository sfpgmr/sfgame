#include "pch.h"
#include "scroll_game.h"
#include "game_obj.h"

namespace sf {
scroll_game::scroll_game() : world_(b2Vec2(0.0f,-40.0f))
{
  {
    b2BodyDef bodydef;
/*
    bodydef.position.Set(0.0f,10.0f);
    ground_body_ = world_.CreateBody(&bodydef);
    b2PolygonShape ground_box;
    ground_box.SetAsBox(20.0f,2.0f);
    ground_fixture_ = ground_body_->CreateFixture(&ground_box,1.0f);
    
    ground_box.SetAsBox(30.0f,2.0f);
    bodydef.position.Set(-60.0f,-50.0f);
    world_.CreateBody(&bodydef)->CreateFixture(&ground_box,1.0f);

    bodydef.position.Set(80.0f,-40.0f);
    world_.CreateBody(&bodydef)->CreateFixture(&ground_box,1.0f);
*/
    float32 xpos[10] = {0.0f,100.0f,110.0f,150.0f,170.0f,200.0f,1000.0f,2000.0f,2010.0f,2500.0f};
    float32 ypos[10] = {-100.0f,-50.0f,-70.0f,-40.0f,-60.0f,-70.0f,-100.0f,-60.0f,-70.0f,-100.0f};
    std::vector<b2Vec2> points;
    for(int i = 0;i < 8 * 10;++i)
    {
      b2Vec2 tmp;
      tmp.x = i * 100.0f;
      tmp.y = ypos[i % 8];
      points.push_back(tmp);
    }

    b2EdgeShape edge;
    b2ChainShape chain;
    chain.CreateChain(&points[0],points.size());
    edge.Set(b2Vec2(0.0f,0.0f),b2Vec2(40.0f,0.0f));
   // ground_box.SetAsBox(20.0f,2.0f);
    bodydef.position.Set(-70.0f,0.0f);
    world_.CreateBody(&bodydef)->CreateFixture(&chain,1.0f);
    scroll_offset_ = 0.0f;
  }

  velocity_iterations_  = 6;
  position_iterations_  = 2;
  time_step_ = 1.0f /60.0f;
  tick_ = 0;
}

void scroll_game::step(float32 delta_time)
{
  static int step_count = 0,index = 0;
  if(!(step_count & 0x07)){
    for(int i = 0;i < objs_.size();++i)
    {
      if(index == objs_.size())
      {
        index = 0;
      }

      if(!objs_[index])
      {
        objs_[index].reset(new game_obj(*this));
        ++index;
        break;
      } else {
        ++index;
      }
    }

  }

  for(int i = 0;i < objs_.size();++i)
  {
    if(objs_[i])
    {
      objs_[i]->move(delta_time);
      if(!objs_[i]->is_used())
      {
        objs_[i].reset(nullptr);
      }
    }
  }
  world_.Step(delta_time,velocity_iterations_,position_iterations_);
  world_.ClearForces();
  step_count++;
  tick_ = GetTickCount64();
  scroll_offset_ += 0.4f;
  screen_actual_aabb_ = screen_aabb_;
  screen_actual_aabb_.lowerBound.x += scroll_offset_;
  screen_actual_aabb_.upperBound.x += scroll_offset_;

  world_.ClearForces();
//  b2Vec2 pos = body_->GetPosition();
//  float32 angle = body_->GetAngle();
//  sf::debug_out(boost::wformat(L"posx:%4.2f posy:%4.2f angle:%4.2f\n") % pos.x % pos.y % angle); 
}

scroll_game::~scroll_game()
{}

void scroll_game::screen_aabb(float width,float height,float scale)
{
  screen_aabb_.lowerBound.Set(-width / scale,-height / scale );
  screen_aabb_.upperBound.Set(width / scale ,height  /scale);
  screen_actual_aabb_ = screen_aabb_;
  screen_actual_aabb_.lowerBound.x += scroll_offset_;
  screen_actual_aabb_.upperBound.x += scroll_offset_;
}

}
