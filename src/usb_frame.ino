#include <TimeLib.h>
#include <TimeAlarms.h>


#include "display.h"
#include "utils.h"

#define PACKET_TYPE_NOTSET 0
#define PACKET_TYPE_SETTINGS 1
#define PACKET_TYPE_CLOCK_SYNC 2
#define PACKET_TYPE_IMAGE 10

#define RCV_TIMEOUT 400  //milliseconds

#define BUF_SIZE 16*3

class CommHandler {

public:
  CommHandler(){
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
        update_leds(buf, offset, BUF_SIZE);
        break;
      default:
        break;
    };
  
  }

  void process_complete_packet(){
    switch(packet_type){
      case PACKET_TYPE_IMAGE:
        repaint();
        break;
      case PACKET_TYPE_CLOCK_SYNC:
        update_time(buf, pos);
        break;
      case PACKET_TYPE_SETTINGS:
        update_settings(buf, pos);
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

  unsigned long rcv_start;
};

/* ***************************************************************** */
/* ***************************  CLOCK  ***************************** */
/* ***************************************************************** */

class Clock{

public:
  Clock::Clock(){
      HH_X = 1;
      HH_Y = 0;
      MM_X = 6;
      MM_Y = 8;
      cmin=-1;
  }

  void draw(){
    int8_t m = minute();
    if(m!=cmin) // only repaint if needed
    {
      fillrect(0,0,16,16, CRGB::Black);
      String hour_str = format_digit(hour());
      draw_string(HH_X,HH_Y,CRGB::DeepSkyBlue, true, hour_str);
      String min_str = format_digit(m);
      draw_string(MM_X,MM_Y,CRGB::GreenYellow, true, min_str);
      repaint();
      cmin = m;
    }
    format_digit(hour());
  }

  
  uint8_t HH_X, HH_Y;
  uint8_t MM_X, MM_Y;

  int8_t cmin; 
};


/* ***************************************************************** */
/* ************************* MAIN SKETCH *************************** */
/* ***************************************************************** */


CommHandler handler;
Clock clk;

AlarmId clock_id;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
//  Serial.begin(230400);
  while (!Serial) ; // Needed for Leonardo only
  Serial.setTimeout(1000);

  init_display();

  setTime(1514589221);
  clock_id = Alarm.timerRepeat(1, clock_mode);
}

void clock_mode(){
  clk.draw();
}



void serialEvent()
{
  digitalWrite(LED_BUILTIN, HIGH);
  while(Serial.available()>0)
  {
    int val = Serial.read();
    if(val>=0)
    {
      handler.process(val);
    }
  }
  digitalWrite(LED_BUILTIN, LOW);
}


void loop() {
  if(handler.isReceiving()){
    handler.check_timeout();
  }
  else{
    Alarm.delay(1); // wait one second between clock display
  }
}
