#pragma once

#include "config.h"
#include "display.h"

#define NIGHT_BRIGHTNESS 5
#define DAY_BRIGHTNESS 10

#define NIGHT_HOUR 23
#define DAY_HOUR 8

#define CLOCK_MODE_STARFIELD 0
#define CLOCK_MODE_SNAKE 1
#define CLOCK_MODE_PONG 2
#define CLOCK_MODE_BREAKOUT 3
#define CLOCK_MODE_MARIO 4
#define CLOCK_MODE_MATRIX 5
#define CLOCK_MODE_FIRE 6
#define CLOCK_MODE_PLAIN 7 // also max
#define CLOCK_MODE_RANDOM 128

#define DEFAULT_CLOCK_MODE CLOCK_MODE_RANDOM

#define DEFAULT_BG_CHANGE 300000 // millis
#define DEFAULT_TIME  1514589221 


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
  Clock(Display *dsp){

      clock_mode = CLOCK_MODE_FIRE;
      last_bg_change = millis() - DEFAULT_BG_CHANGE;
      last_update = 0;
      this->dsp = dsp;
      reset_clock_face();
  }
  void draw_clock_face(){
    switch(clock_mode & 0x0F)
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
        draw_time(Point(2, 0), Point(5, 8),CRGB::White,CRGB::White, true, false, true, false);
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
  int8_t dx,dy;
  int8_t motion;
  uint8_t clock_mode;
  uint8_t colorIndex;
  Display *dsp;
  
  uint8_t snake_head;
  unsigned long last_bg_change;
  unsigned long last_update;
  Point h_points[CLOCK_NUM_POINTS];
  
};

