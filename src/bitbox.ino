#include <TimeLib.h>
#include <Wire.h>
#include <DS1307RTC.h>

#include <stdint.h>
#include <WString.h>



#include "config.h"
#include "display.h"
#include "utils.h"
#include "clock.h"
#include "heart.h"


#define MIN_REPAINT 30
#define CLOCK_UPDATE 125 // redraw clock  at most this often


/* ***************************************************************** */
/* ************************* MAIN SKETCH *************************** */
/* ***************************************************************** */

CommHandler *handler;

Display dsp;
Clock clk(&dsp);


unsigned long last_clock_update=0;
unsigned long last_repaint = 0;

uint8_t heart_fr=0;

unsigned long last_upd=0;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.setTimeout(1000);
  randomSeed(analogRead(0));
  
  // TODO load config from EEPROM

  // init communication handler
  if(Serial) handler = new CommHandler(&dsp);
  else handler = NULL;
  
  setSyncProvider(RTC.get);
  if(timeStatus() != timeSet)
  {
    setTime(DEFAULT_TIME);
    Serial.println("Unable to sync with the RTC");
  }
  else
  {
    Serial.println("RTC has set the system time");
  }
}

void serialEvent()
{
  digitalWrite(LED_BUILTIN, HIGH);
  while(Serial.available()>0)
  {
    int val = Serial.read();
    if(val>=0 && handler)
    {
      handler->process(val);
    }
  }
  digitalWrite(LED_BUILTIN, LOW);
}

void draw_heart(){

  if(millis()-last_upd>100){
    uint8_t har[] = {0,1,1,2,2,2,2,1,1,0};
    last_upd = millis();
    dsp.fillrect(0,0,16,16, CRGB::Black);
    int offset = (har[heart_fr%10])*(heart_width*heart_height*3);
    dsp.drawImage_pm(heart, offset, 0, 0, heart_width, heart_height);    
    dsp.repaint();
    heart_fr+=1;
  }
}


void loop() {
  
  if(handler && handler->isReceiving()){
    handler->check_timeout();
  }
  else if(Serial.available()==0)
  { //only if serial IO is not pending
    unsigned long  t = millis();
    if(day()==28 && month()==4 && hour()>=8)
    {
      draw_heart();
    }
    else
    {
     
      
      if(t-last_clock_update>CLOCK_UPDATE){
        clk.draw();
        last_clock_update = t;
      }   
    } 

    if(t-last_repaint>MIN_REPAINT){
      dsp.flush_buffer();
      last_repaint = t;
    }
//    else{ // temporal dithering gives better colors but also ads a lot of flickering.
//      FastLED.delay(15);
//    }
  }

}

