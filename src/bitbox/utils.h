#pragma once

#include "config.h"
#include "display.h"

// Pong defines
#define PL 1 
#define PR 4
#define PB 6


void flash(int times){
  digitalWrite(LED_BUILTIN, LOW);
  delay(200);  
  int total=200;
  
  int del = total/times;
  for(int i=0;i<times;++i){
    delay(del);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(del);
    digitalWrite(LED_BUILTIN, LOW);      
  }
}


inline String format_digit(int d){
  String s(d);
  if(d < 10)
    return String("0"+s);
  return s;
}


String format_time(){
  return format_digit(hour()) + format_digit(minute());
}


void update_time(byte *buf, int buf_len){
  // read pc time 
  unsigned long t = atol((char*)buf);
  RTC.set(t);   // set the RTC and the system time to the received value
  setTime(t);
}


/* ***************************************************************** */
/* ***************************  CLOCK  ***************************** */
/* ***************************************************************** */

struct Point{
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

      reset_points();
      snake_head = 0;
      colorIndex = 0;
      delta_color = 1;
      motion = 1;
      dx = 1;
      dy = 1;
      last_bg_change = millis();

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
      default: 
        break; // CLOCK_MODE_PLAIN      
    }
  }

  void draw(){
  
    if(cfg->clock_mode & CLOCK_MODE_RANDOM && ((millis() - last_bg_change)> cfg->clock_change_bg))
    {
      reset_points();
      last_bg_change = millis();
      uint8_t new_mode;
      do{
         new_mode = random(CLOCK_MODE_MAX);
      }while(new_mode==cfg->clock_mode&0x0F);

      cfg->clock_mode = new_mode | CLOCK_MODE_RANDOM;
    }
    
    dsp->fadetoblack(0,0,16,16, 128);
    draw_bg();
    
    String hour_str = format_digit(hour());
    String min_str = format_digit(minute());
    dsp->draw_string(hour_str, HH_X,HH_Y,CRGB::SlateGray, false, true);
    dsp->draw_string(min_str, MM_X,MM_Y,CRGB::SteelBlue, false, true);
    dsp->repaint();    
  }

  void draw_starfield(){
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
  }

  void draw_snake(){
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

  }

  void draw_pong(){
    // left pad indices 0,1,2
    // right pad indices 3,4,5
    // ball pad 6 
//    if(h_points[PL].x!=0 || h_points[PR].x!=MATRIX_WIDTH-1) reset_points();

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
  }

  void reset_points(){
    // draw everything
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



/* ***************************************************************** */
/* ***********************  CommHandler **************************** */
/* ***************************************************************** */


class CommHandler {

public:
  CommHandler(Config *cfg, Display *dsp){
    this->cfg = cfg;
    this->dsp = dsp;
    reset();
  }


  void process(byte val){
    if(packet_type==PACKET_TYPE_NOTSET)
    {
      if(val>PACKET_TYPE_NOTSET && val<=PACKET_TYPE_IMAGE)
      {
        packet_type = val;
        rcv_start = millis();
      }
      else{
        Serial.println("Invalid packet type");
      }
      return;
    }

    if(parts==0){
      parts = val;
      if(val==0) {
        Serial.println("Invalid parts number 0.");
        reset();
      }
      return;
    }

    buf[pos] = val;
    pos++;
    
    if(pos==BUF_SIZE)
    {
      process_partial_packet();
      offset += pos;      
      pos = 0;
    }

    if (offset == parts * BUF_SIZE)
    {
      process_complete_packet();
//      String msg = String(packet_type)+" " + String(pos) +"/" + String(parts) + "/" + String(offset)+" Start time: "+ String(rcv_start); 
//      Serial.println(packet_type);
      reset();
    }
  }

  void reset(){
      pos = 0; //reset handler for the next packet
      offset = 0;
      parts=0;
      rcv_start = 0;
      packet_type = PACKET_TYPE_NOTSET;    
  }

  void check_timeout(){
    if(packet_type!=PACKET_TYPE_NOTSET && (millis()-rcv_start)>RCV_TIMEOUT)
    {
//      String msg = String(packet_type)+" " + String(pos) +"/" + String(parts) + "/" + String(offset)+" Start time: "+ String(rcv_start); 
      reset();
      Serial.println("Receive timeout");
    }
  }

  void process_partial_packet(){
    switch(packet_type){
      case PACKET_TYPE_IMAGE:
        dsp->update_leds(buf, offset, BUF_SIZE);
        break;
      default:
        break;
    };
  
  }

  void process_complete_packet(){
    bool res;
    switch(packet_type){
      case PACKET_TYPE_IMAGE:
        dsp->repaint();
        cfg->last_image_received = millis();
        break;
      case PACKET_TYPE_CLOCK_SYNC:
        update_time(buf, pos);
        break;
      case PACKET_TYPE_SETTINGS:
        res = cfg->from_string(String((char*)buf));
        if(!res){
          Serial.println("Failed to parse config");
        }
        
        dsp->reconfigure();

        break;

      default:
        break;
    };
  
  }

  bool isReceiving(){
    return packet_type!=PACKET_TYPE_NOTSET;
  }
  
  byte packet_type;
  uint8_t parts;
  uint16_t pos;
  uint16_t offset;
  byte buf[BUF_SIZE];
  Config *cfg;
  Display *dsp;
  unsigned long rcv_start;
};



