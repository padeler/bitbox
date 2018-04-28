#pragma once

#include "display.h"

#define PACKET_TYPE_NOTSET 0
#define PACKET_TYPE_SETTINGS 1
#define PACKET_TYPE_CLOCK_SYNC 2
#define PACKET_TYPE_IMAGE 10

#define RCV_TIMEOUT 200  //milliseconds

#define BUF_SIZE 16*3


void update_time(byte *buf, int buf_len){
  // read pc time 
  unsigned long t = atol((char*)buf);
  RTC.set(t);   // set the RTC and the system time to the received value
  setTime(t);
}

// FIXME Update to_string and from_string for the config over serial function.
String to_string(uint8_t clock_mode, uint8_t brightness){
  String res = String(clock_mode)   + " " +
                String(brightness)   + " ";

  return res;
}

bool from_string(String str){
  unsigned int start=0, idx=0;
  
  idx = str.indexOf(' ', start); // clock_mode
  if(idx<0) return false;
  uint8_t clock_mode = str.substring(start, idx).toInt();
  start = idx+1;

  idx = str.indexOf(' ', start); // brightness
  if(idx<0) return false;
  uint8_t brightness = str.substring(start, idx).toInt();
  start = idx+1;
  
  return true;
}

/* ***************************************************************** */
/* ***********************  CommHandler **************************** */
/* ***************************************************************** */


class CommHandler {

public:
  CommHandler(Display *dsp){
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
        // cfg->last_image_received = millis();
        break;
      case PACKET_TYPE_CLOCK_SYNC:
        update_time(buf, pos);
        break;
      case PACKET_TYPE_SETTINGS:
        // res = from_string(String((char*)buf));
        // if(!res){
        //   Serial.println("Failed to parse config");
        // }
        
        // dsp->set_brightness(cfg->brightness);

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
  Display *dsp;
  unsigned long rcv_start;
};



