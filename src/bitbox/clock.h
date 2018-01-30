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

#define CLOCK_NUM_POINTS 10
class Clock{

public:
  Clock(Config *cfg, Display *dsp){
      HH_X = 2;
      HH_Y = 0;
      MM_X = 4;
      MM_Y = 7;

      hh=-1;
      mm=-1;

      reset_points();
      snake_head = 0;
      colorIndex = 0;
      delta_color = 1;
      motion = 1;
      dx = 1;
      dy = 1;
      last_bg_change = millis() - cfg->clock_change_bg;

      this->cfg = cfg;
      this->dsp = dsp;
  }
  void draw_bg(){
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
      default: // CLOCK_MODE_PLAIN
        draw_time(Point(HH_X,HH_Y), Point(MM_X, MM_Y), true, false, true, false);
        break;  
    }
  }

  void draw(){
  
    if(cfg->clock_mode & CLOCK_MODE_RANDOM && ((millis() - last_bg_change)> cfg->clock_change_bg))
    {
      reset_points();
      last_bg_change = millis();
      uint8_t new_mode;
      do{
         new_mode = (uint8_t)random(CLOCK_MODE_PLAIN) | CLOCK_MODE_RANDOM;
      }while(new_mode==cfg->clock_mode);

      cfg->clock_mode = new_mode;
    }
    
    draw_bg();
    dsp->repaint();
  }

  void draw_time(Point h_pos, Point m_pos, bool clear_before_draw, bool compact, bool overlay, bool force_redraw){
    int nhh = hour();
    int nmm = minute();

    if(nhh!=hh || nmm!=mm || force_redraw)
    { // only draw if needed.
      String hstr,mstr;
      if(clear_before_draw){
        hstr = format_digit(hh);
        mstr = format_digit(mm);
        dsp->draw_string(hstr, h_pos.x, h_pos.y, CRGB::Black, compact, overlay);
        dsp->draw_string(mstr, m_pos.x, m_pos.y, CRGB::Black, compact, overlay);
      }
      hh = nhh;
      mm = nmm;
      hstr = format_digit(nhh);
      mstr = format_digit(nmm);
      dsp->draw_string(hstr, h_pos.x, h_pos.y, CRGB::Yellow, compact, overlay);
      dsp->draw_string(mstr, m_pos.x, m_pos.y, CRGB::SteelBlue, compact, overlay);
    }
  }

  void draw_starfield(){
    dsp->fadetoblack(0,0,16,16, 128);

    for(int i=0;i<CLOCK_NUM_POINTS;++i){
        h_points[i].x = (h_points[i].x-1);
        dsp->set(h_points[i].x, h_points[i].y, ColorFromPalette(HeatColors_p, colorIndex));
        
        if(h_points[i].x<0){
          h_points[i].x = random(CLOCK_NUM_POINTS*10);
          h_points[i].y = random(CLOCK_NUM_POINTS*10) % MATRIX_HEIGHT;
        }
    }
    colorIndex += delta_color;
    if(colorIndex==0) delta_color = -delta_color;

    draw_time(Point(HH_X,HH_Y), Point(MM_X, MM_Y), false, false, true, true);
  }

  void draw_snake(){
    dsp->fadetoblack(0,0,16,16, 128);

    Point *p = &h_points[snake_head];
    snake_head = (snake_head+1)%CLOCK_NUM_POINTS;
    Point *c = &h_points[snake_head];

    if(random(20)==1) motion = -motion;
    if(random(10)==1) dx = -dx;
    if(random(10)==1) dy = -dy;
    bool route_found=false;
    for(int i=0;i<3;++i)
    {
      c->x = p->x;
      c->y = p->y;
    
      if(motion>0){
        c->x = (c->x + dx);
        if(c->x<0) c->x = p->x;
        else if(c->x>=MATRIX_WIDTH) c->x = p->x;     
      }
      else{
        c->y = (c->y + dy);
        if(c->y<0) c->y = p->y;
        else if(c->y>=MATRIX_HEIGHT) c->y = p->y;     
      }

      CRGB t = dsp->get(c->x, c->y);
      
      if(t[0]==0 && t[1]==0 && t[2]==0){
        route_found=true;
        break;
      }
      // change direction of motion and try again
      if(i==0) motion = -motion;
      dx = -dx;
      dy = -dy;    
    }

    if(!route_found){
      c->x = p->x;
      c->y = p->y;
    }
    
    for(int i=0;i<CLOCK_NUM_POINTS;++i){
      dsp->set(h_points[i].x, h_points[i].y, ColorFromPalette(ForestColors_p, colorIndex));
    }
    
    colorIndex += delta_color;
    if(delta_color==0) delta_color = -delta_color;      

    draw_time(Point(HH_X,HH_Y), Point(MM_X, MM_Y), false, false, true, true);
  }

  void draw_pong(){
    dsp->fadetoblack(0,0,16,16, 128);

    // left pad indices 0,1,2
    // right pad indices 3,4,5
    // ball pad 6 

    // check ball
    if(h_points[PB].x==1 || h_points[PB].x==MATRIX_WIDTH-2)
    {
      dx = -dx;
      if(random(3)==1) dy = -dy;
    }
    if(h_points[PB].y+dy==-1 || h_points[PB].y+dy==MATRIX_HEIGHT){
      dy = -dy;
    }

    h_points[PB].x += dx;
    h_points[PB].y += dy;

    // check pads
    update_pong_pad(PL,0);
    update_pong_pad(PR,MATRIX_WIDTH-1);

    // draw everything
    dsp->fillrect(0,0,1,MATRIX_HEIGHT, CRGB::Black);
    dsp->fillrect(MATRIX_WIDTH-1,0,1,MATRIX_HEIGHT, CRGB::Black);
    for(int8_t i=-1; i<2; i++){
       dsp->set(h_points[PL+i].x, h_points[PL+i].y, CRGB::Green);
       dsp->set(h_points[PR+i].x, h_points[PR+i].y, CRGB::Blue);
    }
    dsp->set(h_points[PB].x, h_points[PB].y, CRGB::Snow);


    draw_time(Point(HH_X,HH_Y), Point(MM_X, MM_Y), false, false, true, true);

  }

  void draw_breakout(){
    dsp->fadetoblack(0,0,16,16, 128);

    // pad indices 0,1,2
    // ball pad 6
    
    Point *b = &h_points[PB];
    CRGB tx = dsp->get(b->x+dx, b->y);
    CRGB ty = dsp->get(b->x, b->y+dy);    
    if(ty[0]!=0 || ty[1]!=0 || ty[2]!=0){
      dy = -dy;
    }
    else if(tx[0]!=0 || tx[1]!=0 || tx[2]!=0){
      dx = -dx;
    }

    // check that ball is in bounds
    if(h_points[PB].x + dx==-1 || h_points[PB].x + dx==MATRIX_WIDTH)
    {
      dx = -dx;
    }
    if(h_points[PB].y+dy==-1 || h_points[PB].y+dy==MATRIX_HEIGHT-1){
      dy = -dy;
      if(random(3)==1) dx = -dx;
    }
    
    h_points[PB].x += dx;
    h_points[PB].y += dy;

    update_breakout_pad(PL, MATRIX_HEIGHT-1);

    // draw everything
    dsp->fillrect(0,MATRIX_HEIGHT-1,MATRIX_WIDTH,1, CRGB::Black);
    for(int8_t i=-1; i<2; i++){
       dsp->set(h_points[PL+i].x, h_points[PL+i].y, CRGB::Purple);
    }
    dsp->set(h_points[PB].x, h_points[PB].y, CRGB::Snow);

    draw_time(Point(0,0), Point(8, 0), false, true, true, true);
  }

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


  void reset_points(){
    for(int8_t i=0; i<CLOCK_NUM_POINTS; i++){
       h_points[i].x = MATRIX_WIDTH/2;
       h_points[i].y = MATRIX_HEIGHT/2;
    }
  }

  void update_pong_pad(int8_t idx, int8_t xpos){
    int8_t goal = MATRIX_HEIGHT/2; // move towards the mid of the screen
    if(abs(h_points[PB].x-h_points[idx].x)<MATRIX_WIDTH/2){ // move towards the ball
      goal = h_points[PB].y;
    }
            
    if(h_points[idx].y<goal){
      h_points[idx].y = min(h_points[idx].y+1,MATRIX_HEIGHT-2);
    }
    else if(h_points[idx].y>goal){
      h_points[idx].y = max(h_points[idx].y-1,1);
    }
    h_points[idx-1].y = h_points[idx].y-1;
    h_points[idx+1].y = h_points[idx].y+1;

    h_points[idx-1].x = xpos;
    h_points[idx].x = xpos;
    h_points[idx+1].x = xpos;
    
  }

  void update_breakout_pad(int8_t idx, int8_t ypos){
    int8_t goal = MATRIX_WIDTH/2; // move towards the mid of the screen
    if(abs(h_points[PB].y-h_points[idx].y)<MATRIX_HEIGHT/2){ // move towards the ball
      goal = h_points[PB].x;
    }
    
    if(h_points[idx].x<goal){
      h_points[idx].x = min(h_points[idx].x+1,MATRIX_WIDTH-2);
    }
    else if(h_points[idx].x>goal){
      h_points[idx].x = max(h_points[idx].x-1,1);
    }
    h_points[idx-1].x = h_points[idx].x-1;
    h_points[idx+1].x = h_points[idx].x+1;

    h_points[idx-1].y = ypos;
    h_points[idx].y = ypos;
    h_points[idx+1].y = ypos;
    
  }

  int hh,mm;
  
  int8_t delta_color;
  int8_t dx,dy, motion;
  uint8_t colorIndex;
  Config *cfg;
  Display *dsp;
  
  uint8_t HH_X, HH_Y;
  uint8_t MM_X, MM_Y;

  uint8_t snake_head;
  unsigned long last_bg_change;
  
  Point h_points[CLOCK_NUM_POINTS];
  
};

