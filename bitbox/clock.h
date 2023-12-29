#pragma once

#include "display.h"
#include "transitions.h"

#define NIGHT_BRIGHTNESS 5
#define DAY_BRIGHTNESS 10

#define NIGHT_HOUR 23
#define DAY_HOUR 8

#define CLOCK_MODE_STARFIELD 0
#define CLOCK_MODE_SNAKE 1
#define CLOCK_MODE_PONG 2
#define CLOCK_MODE_BREAKOUT 3
#define CLOCK_MODE_MATRIX 4
#define CLOCK_MODE_FIRE 5
#define CLOCK_MODE_MARIO 6
#define CLOCK_MODE_PLAIN 7 // also max
#define CLOCK_MODE_RANDOM 128

#define DEFAULT_CLOCK_MODE CLOCK_MODE_RANDOM

#define DEFAULT_BG_CHANGE 180000 // millis

#define CLOCKFACE_MS_PER_FRAME 125 // redraw clock  at most this often


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


class ClockFace: public Animation
{
public:

  ClockFace(uint8_t mode): clock_mode(mode)
  {
    hh = -1;
    mm = -1;
    last_repaint = 0;
  }

  void draw_time(Display *dsp, const Point &h_pos, const Point &m_pos, const CRGB &h_color, const CRGB &m_color, bool clear_before_draw, bool compact, bool overlay, bool force_redraw);
  
  virtual ~ClockFace() throw() {}
  virtual bool update_clock_face(Display *dsp, bool force_redraw);

  bool update(Display *dsp, bool force_redraw)
  {
    // clear screen on first run
    if(last_repaint==0 || force_redraw) dsp->fillrect(0,0,16,16, CRGB::Black);

    if(millis()-last_repaint>CLOCKFACE_MS_PER_FRAME || force_redraw){
      update_clock_face(dsp, force_redraw);
      dsp->repaint();
      last_repaint = millis();
    }
    return false; // ClockFace animations never end (its a clock)
  }

  
  uint8_t clock_mode;
  int hh, mm;
  unsigned long last_repaint;
};

class Starfield: public ClockFace
{
public:
  #define NUM_STARS 10
  Starfield(): ClockFace(CLOCK_MODE_STARFIELD)
  {
    colorIndex = 0;
    delta_color = 1;
    vertical = false;

    for (int8_t i = 0; i < NUM_STARS; i++)
    {
      h_points[i].x = MATRIX_WIDTH / 2;
      h_points[i].y = MATRIX_HEIGHT / 2;
    }
  }
  
  bool update_clock_face(Display *dsp, bool force_redraw);

  int8_t delta_color;
  uint8_t colorIndex;
  bool vertical;


  Point h_points[NUM_STARS];
};

class Breakout: public ClockFace
{
public:
  Breakout(): ClockFace(CLOCK_MODE_BREAKOUT)
  {
    dx = dy = 1;
    ball.x=MATRIX_WIDTH/2;
    ball.y=MATRIX_HEIGHT-3;
    pad.x = MATRIX_WIDTH/2;
    pad.y = MATRIX_HEIGHT-1;
    ball_bg = CRGB::Black;
  }

  void update_breakout_pad();
  bool update_clock_face(Display *dsp, bool force_redraw);

  int8_t dx,dy;
  Point pad, ball;
  CRGB ball_bg;
};

class MultiClockFace : public ClockFace
{
public:
  MultiClockFace(uint8_t mode): ClockFace(mode)
  {

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



  bool update_clock_face(Display *dsp, bool force_redraw)
  {
    switch (clock_mode & 0x0F)
    {
    case CLOCK_MODE_SNAKE:
      draw_snake(dsp, force_redraw);
      break;
    case CLOCK_MODE_PONG:
      draw_pong(dsp, force_redraw);
      break;
    case CLOCK_MODE_MARIO:
      draw_mario(dsp);
      break;
    case CLOCK_MODE_MATRIX:
      draw_matrix(dsp, force_redraw);
      break;
    case CLOCK_MODE_FIRE:
      draw_fire(dsp, force_redraw);
      break;
    default: // CLOCK_MODE_PLAIN
      draw_time(dsp, Point(2, 0), Point(5, 8), CRGB::White, CRGB::White, true, false, true, force_redraw);
      break;
    }
    return false;
  }


  void draw_matrix(Display *dsp, bool force_redraw);
  void draw_mario(Display *dsp);
  void draw_fire(Display *dsp, bool force_redraw);
  void draw_starfield(Display *dsp, bool force_redraw);
  void draw_snake(Display *dsp, bool force_redraw);
  void draw_pong(Display *dsp, bool force_redraw);
  void update_pong_pad(int8_t idx, int8_t xpos);

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

  int8_t delta_color;
  int8_t dx, dy;
  int8_t motion;
  uint8_t colorIndex;
  unsigned long anim_frame;

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

    if(current_face!=NULL && millis() - last_bg_change<DEFAULT_BG_CHANGE) return; 
    
    uint8_t clock_mode;
    if(current_face!=NULL) clock_mode = current_face->clock_mode;
    else clock_mode = DEFAULT_CLOCK_MODE & 0xFF;
    
    if(hour() >= DAY_HOUR && hour() <= NIGHT_HOUR)
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
      // NOTE: no need to delete old clock face, the animation is deleted on pop.
      dsp->animation_delete(current_face); 
      current_face = create_face_animation(clock_mode);
      dsp->animation_push(current_face);

      if(random8(0,2)==0)
        dsp->animation_push(new Collapse()); // transition
      else
        dsp->animation_push(new Melt()); // transition

    }
  }

  ClockFace *create_face_animation(uint8_t clock_mode)
  {
    switch (clock_mode & 0x0F)
    {
      case CLOCK_MODE_BREAKOUT:
        return new Breakout();
      case CLOCK_MODE_STARFIELD:
        return new Starfield();
      default: // multi face clock anim  
        return new MultiClockFace(clock_mode);
    }
  }
  
  unsigned long last_bg_change;
  ClockFace *current_face;
};
