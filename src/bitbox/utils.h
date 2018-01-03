#pragma once

#include "config.h"
#include "display.h"



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
      HH_X = 1;
      HH_Y = 0;
      MM_X = 5;
      MM_Y = 8;
      

      for(int i=0;i<CLOCK_NUM_POINTS;++i)
      {       
        h_points[i].x = 0;
        h_points[i].y = i % MATRIX_HEIGHT;
      }
      heatIndex = 0;
      delta_heat = 1;

      this->cfg = cfg;
      this->dsp = dsp;
  }

  void draw(){
  
    dsp->fadetoblack(0,0,16,16, 128);
    draw_points();
    
    String hour_str = format_digit(hour());
    String min_str = format_digit(minute());
    dsp->draw_string(hour_str, HH_X,HH_Y,CRGB::SlateGray, false, true);
    dsp->draw_string(min_str, MM_X,MM_Y,CRGB::SteelBlue, false, true);
    dsp->repaint();    
  }

  void draw_points(){
    for(int i=0;i<CLOCK_NUM_POINTS;++i){
        h_points[i].x = (h_points[i].x+1);
        dsp->set_led(h_points[i].x, h_points[i].y, ColorFromPalette(HeatColors_p, heatIndex));
        
        if(h_points[i].x>MATRIX_WIDTH){
          h_points[i].x = -random(CLOCK_NUM_POINTS*10);
          h_points[i].y = random(CLOCK_NUM_POINTS*10) % MATRIX_HEIGHT;
        }
    }
    heatIndex += delta_heat;
    if(heatIndex==0) delta_heat = -delta_heat;
    
  }
  int8_t delta_heat;
  uint8_t heatIndex;
  Config *cfg;
  Display *dsp;
  
  uint8_t HH_X, HH_Y;
  uint8_t MM_X, MM_Y;
 
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



