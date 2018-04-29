#pragma once

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
#define DEFAULT_TIME 1514589221

#define CLOCK_FACE_FRAMERATE 125 // redraw clock  at most this often


// Pong defines
#define PL 1
#define PR 4
#define PB 6

#define CLOCK_NUM_POINTS 8

struct Point
{
  Point(){};
  Point(int x, int y) : x(x), y(y){};
  int8_t x, y;
};

class ClockFace : public Animation
{
public:
  ClockFace(uint8_t mode)
  {
    clock_mode = mode;
    hh = -1;
    mm = -1;
    snake_head = 0;
    colorIndex = 0;
    delta_color = 1;
    motion = 1;
    dx = 1;
    dy = 1;
    last_repaint = 0;

    for (int8_t i = 0; i < CLOCK_NUM_POINTS; i++)
    {
      h_points[i].x = MATRIX_WIDTH / 2;
      h_points[i].y = MATRIX_HEIGHT / 2;
    }
  }

  ~ClockFace() throw(){}

  bool update(Display *dsp)
  {
    // clear screen on first run
    if(last_repaint==0) dsp->fillrect(0,0,16,16, CRGB::Black);

    if(millis()-last_repaint>CLOCK_FACE_FRAMERATE){
      draw_clock_face(dsp);
      dsp->repaint(); 
      last_repaint = millis();
    }
    return false; // ClockFace animations never end (its a clock)
  }


  void draw_clock_face(Display *dsp)
  {
    switch (clock_mode & 0x0F)
    {
    case CLOCK_MODE_STARFIELD:
      draw_starfield(dsp);
      break;
    case CLOCK_MODE_SNAKE:
      draw_snake(dsp);
      break;
    case CLOCK_MODE_PONG:
      draw_pong(dsp);
      break;
    case CLOCK_MODE_BREAKOUT:
      draw_breakout(dsp);
      break;
    case CLOCK_MODE_MARIO:
      draw_mario(dsp);
      break;
    case CLOCK_MODE_MATRIX:
      draw_matrix(dsp);
      break;
    case CLOCK_MODE_FIRE:
      draw_fire(dsp);
      break;
    default: // CLOCK_MODE_PLAIN
      draw_time(dsp, Point(2, 0), Point(5, 8), CRGB::White, CRGB::White, true, false, true, false);
      break;
    }
  }

  void draw_time(Display *dsp, const Point &h_pos, const Point &m_pos, const CRGB &h_color, const CRGB &m_color, bool clear_before_draw, bool compact, bool overlay, bool force_redraw);

  void draw_matrix(Display *dsp);
  void draw_mario(Display *dsp);
  void draw_fire(Display *dsp);
  void draw_starfield(Display *dsp);
  void draw_snake(Display *dsp);
  void draw_pong(Display *dsp);
  void draw_breakout(Display *dsp);
  void update_pong_pad(int8_t idx, int8_t xpos);
  void update_breakout_pad(int8_t idx, int8_t ypos);

  void clear_points(Display *dsp)
  {
    for (int8_t i = 0; i < CLOCK_NUM_POINTS; i++)
    {
      dsp->set(h_points[i].x, h_points[i].y, CRGB::Black);
    }
  }

  void fade_points(Display *dsp, uint8_t amount)
  {
    for (int8_t i = 0; i < CLOCK_NUM_POINTS; i++)
    {
      dsp->get(h_points[i].x, h_points[i].y).nscale8(amount);
    }
  }

  int hh, mm;
  int8_t delta_color;
  int8_t dx, dy;
  int8_t motion;
  uint8_t clock_mode;
  uint8_t colorIndex;
  unsigned long last_repaint, anim_frame;

  uint8_t snake_head;
  Point h_points[CLOCK_NUM_POINTS];

};


/* ***************************************************************** */
/* ***************************  CLOCK  ***************************** */
/* ***************************************************************** */

class Clock
{

public:
  Clock()
  {
    last_bg_change = 0;
    current_face = NULL;
  }

  void update_clock_face(Display *dsp)
  {
    uint8_t clock_mode;

    if(current_face!=NULL && millis() - last_bg_change<DEFAULT_BG_CHANGE) return; 
    
    if (hour() >= DAY_HOUR && hour() <= NIGHT_HOUR)
    {
      dsp->set_brightness(DAY_BRIGHTNESS);

      if(DEFAULT_CLOCK_MODE & CLOCK_MODE_RANDOM)
      {
        clock_mode = (clock_mode + (uint8_t)random(CLOCK_MODE_PLAIN - 1)) % CLOCK_MODE_PLAIN;
      }
    }
    else
    { // night time, use plain clock mode
      dsp->set_brightness(NIGHT_BRIGHTNESS);
      clock_mode = CLOCK_MODE_PLAIN;
    }

    last_bg_change = millis();

    if(current_face==NULL || current_face->clock_mode!=clock_mode)
    { // remove old clock face and add a new one. 
      dsp->animation_delete(current_face); 
      
      current_face = new ClockFace(clock_mode);
      dsp->animation_push(current_face);
      // NOTE: no need to delete old clock face, the animation is deleted on pop.
    }
  }
  
  unsigned long last_bg_change;
  ClockFace *current_face;
};
