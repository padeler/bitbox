#include <TimeLib.h>

#include "config.h"
#include "display.h"
#include "utils.h"



/* ***************************************************************** */
/* ************************* MAIN SKETCH *************************** */
/* ***************************************************************** */


Config cfg;

Display *dsp;
CommHandler *handler;
Clock *clk;

unsigned long last_clock_update=0;
unsigned long last_repaint = 0;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
//  Serial.begin(230400);
  while (!Serial) ; // Needed for Leonardo only
  Serial.setTimeout(1000);
  
  randomSeed(analogRead(0));
  
  // TODO load config from EEPROM

  // init display.
  dsp = new Display(&cfg);

  // init clock
  clk = new Clock(&cfg, dsp);

  // init communication handler
  handler = new CommHandler(&cfg, dsp);
  
  setTime(cfg.sync_time);
}

void serialEvent()
{
  digitalWrite(LED_BUILTIN, HIGH);
  while(Serial.available()>0)
  {
    int val = Serial.read();
    if(val>=0)
    {
      handler->process(val);
    }
  }
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  
  
  if(handler->isReceiving()){
    handler->check_timeout();
  }
  
  else if(Serial.available()==0){ //only if serial IO is not pending
    unsigned long  t = millis();
    
    if(t-last_clock_update>CLOCK_UPDATE && abs(t-cfg.last_image_received)>CLOCK_SLEEP){
      clk->draw();
      last_clock_update = t;
    }
    if(t-last_repaint>MIN_REPAINT){
      dsp->flush_buffer();
      last_repaint = t;
    }
  }
}

