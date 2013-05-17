#include "pch.h"
#include "scroll_test.h"

namespace sf {
scroll_test::scroll_test() : world_(b2Vec2(0.0f,-40.0f))
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

  {
    b2BodyDef body_def;
    body_def.type = b2_dynamicBody;
    body_def.position.Set(0.0f,100.0f);
    body_ = world_.CreateBody(&body_def);
    b2PolygonShape dynamic_box;
    dynamic_box.SetAsBox(4.0f,4.0f);
    b2FixtureDef fixture_def;
    fixture_def.shape = & dynamic_box;
    fixture_def.density = 1.0f;
    fixture_def.friction = 0.3f;
    body_fixture_ = body_->CreateFixture(&fixture_def);

  }

  velocity_iterations_  = 6;
  position_iterations_  = 2;
  time_step_ = 1.0f /60.0f;
  tick_ = 0;
}

void scroll_test::step()
{
  // タイミングの調整
  if(tick_ != 0 ) 
  {
    ULONGLONG elapsed = GetTickCount64();
    if(elapsed < tick_)
    {
      elapsed = 0xFFFFFFFFFFFFFFFF - tick_ + elapsed;
//      time_step_ = (float32)(0xFFFFFFFFFFFFFFFF - tick_ + elapsed) / 1000.0f;
    } else {
      elapsed = elapsed - tick_;
//      time_step_ = (float32)(elapsed - tick_) / 1000.0f;
    }

    if(elapsed < 32)
    {
      //Sleep(32 - elapsed);
      time_step_ = 32.0f/1000.0f;

    } else {
      time_step_ = time_step_ = (float32)(elapsed) / 1000.0f;
    }
  }
  world_.Step(time_step_,velocity_iterations_,position_iterations_);
  static int step_count = 0,max_count = 0;
  if((!(step_count & 0x1f)) && (max_count < 100) ){
    max_count++;
    b2BodyDef body_def;
    body_def.type = b2_dynamicBody;
    body_def.position.Set(scroll_offset_ + (float32)(rand() & 0x7fff) / 32767.0f * 20.0f - 2.5f + 120.0f  ,90.0f);
    body_ = world_.CreateBody(&body_def);
    float32 w = (float32)(rand() & 0x7fff) / 32767.0f * 13.5f + 0.5f;
    b2FixtureDef fixture_def;
    fixture_def.density = 1.0f;
    fixture_def.friction = 0.5f;

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
  step_count++;
  tick_ = GetTickCount64();
  scroll_offset_ += 0.4f;
  world_.ClearForces();
//  b2Vec2 pos = body_->GetPosition();
//  float32 angle = body_->GetAngle();
//  sf::debug_out(boost::wformat(L"posx:%4.2f posy:%4.2f angle:%4.2f\n") % pos.x % pos.y % angle); 
}

scroll_test::~scroll_test()
{
}
}
