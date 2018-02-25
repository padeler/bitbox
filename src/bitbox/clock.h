#pragma once

#include "config.h"
#include "display.h"

// Pong defines
#define PL 1 
#define PR 4
#define PB 6


/* ***************************************************************** */
/* ***************************  CLOCK  ***************************** */
/* ***************************************************************** */

struct Point{
  Point(){};
  Point(int x, int y):x(x),y(y)
  {};
  int8_t x,y;
};

#define CLOCK_NUM_POINTS 8
class Clock{

public:
  Clock(Config *cfg, Display *dsp){
    
      reset_clock_face();

      last_bg_change = millis() - cfg->clock_change_bg;
      last_update = 0;
      this->cfg = cfg;
      this->dsp = dsp;
  }
  void draw_clock_face(){
    switch(cfg->clock_mode & 0x0F)
    {
      case CLOCK_MODE_STARFIELD:
        draw_starfield();
        break;
      case CLOCK_MODE_SNAKE:
        draw_snake();
        break;
      case CLOCK_MODE_PONG:
        draw_pong();
        break;
      case CLOCK_MODE_BREAKOUT:
        draw_breakout();
        break;
      case CLOCK_MODE_MARIO:
        draw_mario(); 
        break; 
       case CLOCK_MODE_MATRIX:
        draw_matrix();
        break;
      case CLOCK_MODE_FIRE:
        draw_fire();
        break;
      default: // CLOCK_MODE_PLAIN
        draw_time(Point(2, 0), Point(4, 7),CRGB::Yellow,CRGB::SteelBlue, true, false, true, false);
        break;  
    }
  }

  void draw();
  void draw_time(const Point &h_pos, const Point &m_pos, const CRGB &h_color, const CRGB &m_color, bool clear_before_draw, bool compact, bool overlay, bool force_redraw);

  void draw_matrix();
  void draw_mario();
  void draw_fire();
  void draw_starfield();
  void draw_snake();
  void draw_pong();
  void draw_breakout();
  
  void clear_points(){
    for(int8_t i=0; i<CLOCK_NUM_POINTS; i++){
      dsp->set(h_points[i].x, h_points[i].y, CRGB::Black);
    }    
  }

  void fade_points( uint8_t amount){
    for(int8_t i=0; i<CLOCK_NUM_POINTS; i++){
      dsp->get(h_points[i].x, h_points[i].y).nscale8(amount);
    }    
  }

  void reset_clock_face();
  void update_pong_pad(int8_t idx, int8_t xpos);
  void update_breakout_pad(int8_t idx, int8_t ypos);

  
  int hh,mm;
  int8_t delta_color;
  int8_t dx,dy, motion;
  uint8_t colorIndex;
  Config *cfg;
  Display *dsp;
  
  uint8_t snake_head;
  unsigned long last_bg_change;
  unsigned long last_update;
  Point h_points[CLOCK_NUM_POINTS];
  
};

