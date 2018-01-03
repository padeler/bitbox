#pragma once 

#include <stdint.h>
#include <WString.h>

#define CLOCK_MODE_NORMAL 0
#define CLOCK_MODE_OVERLAY 2


#define PACKET_TYPE_NOTSET 0
#define PACKET_TYPE_SETTINGS 1
#define PACKET_TYPE_CLOCK_SYNC 2
#define PACKET_TYPE_IMAGE 10

#define RCV_TIMEOUT 200  //milliseconds

#define BUF_SIZE 16*3

#define MIN_REPAINT 30
#define CLOCK_SLEEP 1000 // wait this amount of millis from last image received from serial.
#define CLOCK_UPDATE 100 // redraw clock  at most this often

#define DEFAULT_TIME  1514589221 

class Config
{
public:
  unsigned long default_time, sync_time;
  uint8_t clock_mode;
  uint8_t brightness;
  unsigned int last_image_received;
 
  Config(){
    sync_time = DEFAULT_TIME;
    clock_mode = CLOCK_MODE_NORMAL;
    brightness = 20;
    last_image_received = 0;
  }

  String to_string(){
    String res = String(sync_time) + " " +
                 String(clock_mode)   + " " +
                 String(brightness)   + " ";

    return res;
  }

  bool from_string(String str){
    unsigned int start=0, idx=0;
    
    idx = str.indexOf(' ', start); // time
    if(idx<0) return false;
    sync_time = str.substring(start, idx).toInt();
    start = idx+1;

    idx = str.indexOf(' ', start); // clock_mode
    if(idx<0) return false;
    clock_mode = str.substring(start, idx).toInt();
    start = idx+1;

    idx = str.indexOf(' ', start); // brightness
    if(idx<0) return false;
    brightness = str.substring(start, idx).toInt();
    start = idx+1;
    
    return true;
  }
};
