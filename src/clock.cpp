#include <TimeLib.h>

#include "clock.h"
#include "fire.h"
// #include "mario.h"

inline String format_digit(int d){
  String s(d);
  if(d < 10)
    return String("0"+s);
  return s;
}

void ClockFace::draw_time(Display *dsp, const Point &h_pos, const Point &m_pos, const CRGB &h_color, const CRGB &m_color, bool clear_before_draw, bool compact, bool overlay, bool force_redraw){
  int nhh = hour();
  int nmm = minute();

  if(nhh!=hh || nmm!=mm || force_redraw)
  { // only draw if needed.
    String hstr,mstr;
    if(clear_before_draw){
      hstr = format_digit(hh);
      mstr = format_digit(mm);
      dsp->draw_string(hstr, h_pos.x, h_pos.y, CRGB::Black, compact, false);
      dsp->draw_string(mstr, m_pos.x, m_pos.y, CRGB::Black, compact, false);
    }
    hh = nhh;
    mm = nmm;
    hstr = format_digit(nhh);
    mstr = format_digit(nmm);
    dsp->draw_string(hstr, h_pos.x, h_pos.y, h_color, compact, overlay);
    dsp->draw_string(mstr, m_pos.x, m_pos.y, m_color, compact, overlay);
  }
}


void MultiClockFace::update_pong_pad(int8_t idx, int8_t xpos){
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

void Breakout::update_breakout_pad(){
  int8_t goal = MATRIX_WIDTH/2; // move towards the mid of the screen
  if(abs(ball.y-pad.y)<MATRIX_HEIGHT/2){ // move towards the ball
    goal = ball.x;
  }
  
  if(pad.x<goal){
    pad.x = min(pad.x+abs(dx),MATRIX_WIDTH-2);
  }
  else if(pad.x>goal){
    pad.x = max(pad.x-abs(dx),1);
  }
  pad.y = MATRIX_HEIGHT-1;
}



/* ************ MATRIX FACE ************** */
void MultiClockFace::draw_matrix(Display *dsp, bool force_redraw){
  dsp->fadetoblack(0,0,16,16, 253);
  for(int i=0;i<CLOCK_NUM_POINTS;++i){
      h_points[i].y = (h_points[i].y+1);
      dsp->set(h_points[i].x, h_points[i].y, ColorFromPalette(ForestColors_p, colorIndex));
      if(h_points[i].y>MATRIX_HEIGHT){
        h_points[i].x = random(CLOCK_NUM_POINTS*3) % MATRIX_WIDTH;
        h_points[i].y = -1 - random(CLOCK_NUM_POINTS);
      }
  }
  colorIndex = (colorIndex + delta_color) % 44;
  draw_time(dsp, Point(0, 0), Point(5, 8), CRGB(0x85FF85), CRGB(0x70FF70), true, false, false, true);
}

/* ************ MARIOFACE ************** */
// void MultiClockFace::draw_mario(Display *dsp, bool force_redraw){
//   dsp->fillrect(0,0,16,16, CRGB::Black);
//   int offset = (motion%mario_frames)*(mario_width*mario_height*3);

//   dsp->drawImage_pm(mario, offset, 2,2, mario_width, mario_height);    
//   draw_time(dsp, Point(0,0), Point(0, 8), CRGB(0x35FF35), CRGB(0x35FF35), false, false, false, true);

//   if(millis()-anim_frame>250){
    
//     if(motion+dx>=mario_frames || motion+dx<0){
//       dx = -dx;
//     }
    
//     motion += dx;
//     anim_frame = millis();
//   }
// }

/* ************ FIRE FACE ************** */
void MultiClockFace::draw_fire(Display *dsp, bool force_redraw){
  dsp->fadetoblack(0,0,16,16, 200);
  int offset = (motion%fire_frames)*(fire_width*fire_height*3);

  dsp->drawImage_pm(fire, offset, 0,0, fire_width, fire_height);    
  draw_time(dsp, Point(0,0), Point(8, 0),CRGB::Yellow,CRGB::SteelBlue, true, true, true, true);

  if(millis()-anim_frame>100){
    motion = (motion + (int)dx) % fire_frames;
    anim_frame = millis();
  }
}

/* ************ STARFIELD FACE ************** */
bool Starfield::update_clock_face(Display *dsp, bool force_redraw){
  dsp->fadetoblack(0,0,16,16, 128);

  for(int i=0;i<NUM_STARS;++i){
    if(vertical) h_points[i].y = (h_points[i].y+1);
    else h_points[i].x = (h_points[i].x-1);

    dsp->set(h_points[i].x, h_points[i].y, ColorFromPalette(HeatColors_p, colorIndex));

    if(vertical){
      if(h_points[i].y>MATRIX_HEIGHT){
        h_points[i].x = random(NUM_STARS*10) % MATRIX_WIDTH;
        h_points[i].y = - random(NUM_STARS);
      }
    }
    else{
      if(h_points[i].x<0){
        h_points[i].x = random(NUM_STARS*10);
        h_points[i].y = random(NUM_STARS*10) % MATRIX_HEIGHT;
      }
    }
  }
  colorIndex += delta_color;
  if(colorIndex==0){
    delta_color = -delta_color;
    if(random(2)==0) vertical=!vertical;
  }

  draw_time(dsp, Point(2, 0), Point(4, 7), CRGB::Yellow, CRGB::SteelBlue, false, false, false, true);
  
  return false; //never completes.

}

/* ************ SNAKE FACE ************** */
void MultiClockFace::draw_snake(Display *dsp, bool force_redraw){
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

  draw_time(dsp, Point(2,0), Point(4, 7),CRGB::Yellow,CRGB::SteelBlue, false, false, true, true);
}

/* ************ PONG FACE ************** */
void MultiClockFace::draw_pong(Display *dsp, bool force_redraw){
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
     dsp->set(h_points[PL+i].x, h_points[PL+i].y, CRGB::Yellow);
     dsp->set(h_points[PR+i].x, h_points[PR+i].y, CRGB::SteelBlue);
  }
  dsp->set(h_points[PB].x, h_points[PB].y, CRGB(0xf93500));

  draw_time(dsp, Point(2,0), Point(4, 7),CRGB::Yellow,CRGB::SteelBlue, false, false, true, true);
}


/* ************ BREAKOUT FACE ************** */

bool break_block(CRGB &bl){
  int v = (int)bl[0]+(int)bl[1]+int(bl[2]);
  if(v>0){ // there is a block with above min color.
    if(v>100) bl.nscale8(200); // change color (make dimmer)
    return true;
  }
  return false;
}
    
float update_speed(int dx, int base)
{
  if(random(10)%3==0) base = 2*base;
  return -dx/abs(dx) * base;
}

bool Breakout::update_clock_face(Display *dsp, bool force_redraw){
  // erase ball
  dsp->set(ball.x, ball.y, CRGB::Black);

  Point *b = &ball;
  if(b->y<=8) // check for block collistions
  {
    CRGB tx = dsp->get(b->x+dx, b->y);
    CRGB ty = dsp->get(b->x, b->y+dy);    
    CRGB t = dsp->get(b->x+dx, b->y+dy);    
    
    bool flipx = false;
    bool flipy = false;
    if(break_block(ty)){
      dsp->set(b->x, b->y+dy, ty);
      flipy = true;
    }
    if(break_block(tx)){
      dsp->set(b->x+dx, b->y, tx);
      flipx = true;
    }
    if(break_block(t)){
      dsp->set(b->x+dx, b->y+dy, t);
      flipx = flipy = true;
    }

    if(flipx) dx = -dx;
    if(flipy) dy = -dy;
  }

  
  // check that ball is in bounds
  if(ball.y+dy<0){
    dy = -dy;
  }

  if(ball.y+dy>MATRIX_HEIGHT-2){
    if(random(3)==1) {
      dx = update_speed(dx, 1);
    }
    dy = -dy;
  }

  if(ball.x + dx<0 || ball.x + dx>MATRIX_WIDTH-1)
  {
    dx = -dx;
  }
  
  ball.x += dx;
  ball.y += dy;

  update_breakout_pad();

  // draw everything
  dsp->fillrect(0,MATRIX_HEIGHT-1,MATRIX_WIDTH,1, CRGB::Black);
  dsp->fillrect(pad.x-1,pad.y,3,1, CRGB::Purple);
  dsp->set(ball.x, ball.y, CRGB::Snow);

  draw_time(dsp, Point(0,0), Point(8, 0),CRGB::Yellow,CRGB::SteelBlue, true, true, true, force_redraw);

  return false;
}

