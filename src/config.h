#pragma once 

#include <TimeLib.h>
#include <Wire.h>
#include <DS1307RTC.h>

#include <stdint.h>
#include <WString.h>


#define MAX_BRIGHTNESS 30 
#define NIGHT_BRIGHTNESS 5
#define DAY_BRIGHTNESS 10

#define NIGHT_HOUR 23
#define DAY_HOUR 8

#define PACKET_TYPE_NOTSET 0
#define PACKET_TYPE_SETTINGS 1
#define PACKET_TYPE_CLOCK_SYNC 2
#define PACKET_TYPE_IMAGE 10

#define RCV_TIMEOUT 200  //milliseconds

#define BUF_SIZE 16*3

#define MIN_REPAINT 30
#define CLOCK_SLEEP 2000 // wait this amount of millis from last image received from serial.
#define CLOCK_UPDATE 125 // redraw clock  at most this often

#define CLOCK_MODE_STARFIELD 0
#define CLOCK_MODE_SNAKE 1
#define CLOCK_MODE_PONG 2
#define CLOCK_MODE_BREAKOUT 3
#define CLOCK_MODE_MARIO 4
#define CLOCK_MODE_MATRIX 5
#define CLOCK_MODE_FIRE 6
#define CLOCK_MODE_PLAIN 7 // also max
#define CLOCK_MODE_RANDOM 128



#define DEFAULT_TIME  1514589221 

class Config
{
public:
  unsigned long clock_change_bg;// if clock_mode&CLOCK_MODE_RANDOM then change bg every this amount of millis
  uint8_t clock_mode;
  uint8_t brightness;
  unsigned int last_image_received;
 
  Config(){
    clock_mode = CLOCK_MODE_RANDOM;
    clock_change_bg = 300000; 
    brightness = DAY_BRIGHTNESS;
    last_image_received = 0;
  }

  String to_string(){
    String res = String(clock_mode)   + " " +
                 String(brightness)   + " ";

    return res;
  }

  bool from_string(String str){
    unsigned int start=0, idx=0;
    
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
