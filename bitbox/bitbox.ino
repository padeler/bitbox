#include <TimeLib.h>
#include <Wire.h>
#include <DS1307RTC.h>

// Including <SD.h> caused a crash on boot probably memory issue on the arduino
// Older versions used to work (circa 2019). Probably wont be an issue with stronger hardware.
// #include <SD.h>

#include <stdint.h>
#include <WString.h>

#include "display.h"
#include "utils.h"
#include "clock.h"
// #include "heart.h"

#include "transitions.h"

#include "message.h"

// #include "Adafruit_Sensor.h"
// #include "DHT.h"

#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define DHTPIN 7

#define SS_PIN 4 // SD CARD SS PIN

#define TARGET_MS_PER_FRAME 30
#define DEFAULT_TIME 1514589221


/* ***************************************************************** */
/* ************************* MAIN SKETCH *************************** */
/* ***************************************************************** */

CommHandler *handler = NULL;

Display *dsp;
Clock *clk;


unsigned long last_repaint = 0;

// uint8_t heart_fr=0;

unsigned long last_upd=0;

// DHT dht(DHTPIN, DHTTYPE);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  // Serial.begin(115200);
  Serial.begin(9600);
  Serial.setTimeout(1000);
  randomSeed(analogRead(0));
  
  // TODO load config from EEPROM
  
  // Initialize thermometer/humidity lib
  // dht.begin();

  // initialize display
  dsp = new Display();
  
  // initialize clock
  clk = new Clock();

  // init communication handler
  if(Serial) handler = new CommHandler(dsp);
  else handler = NULL;
  
  setSyncProvider(RTC.get);
  if(timeStatus() != timeSet)
  {
    setTime(DEFAULT_TIME);
    Serial.println(F("Unable to sync with the RTC"));
  }
  else
  {
    Serial.println(F("RTC has set the system time"));
  }


  // Serial.print(F("Initializing SD card..."));
  // if (!SD.begin(SS_PIN)) {
  //   Serial.println(F("failed!"));
  //   return;
  // }
  // Serial.println(F("OK!"));

  // initialize first clock face
  clk->update_clock_face(dsp);
  // add a transition
  // dsp->animation_push(new Melt());  
  // push a text message animation
  Message *msg = new Message(F("bitbox"), 16,5,-1,0,false);
  dsp->animation_push(msg); 
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



// For debugging
// bool blink=false;
// byte i=0,j=0;

void loop() {
  
  // CRGB colors[4] = { CRGB::Red, CRGB::Green, CRGB::Blue , CRGB::Black};
  // dsp->set(i%16, i/16,colors[j%4]);
  // dsp->repaint();
  // i++;
  // if(i==0) j++;
  
  if(handler && handler->isReceiving()){
    handler->check_timeout();
  }
  else if(Serial.available()==0)
  { //only if serial IO is not pending


    // if(day()==28 && month()==4 && hour()>=8)
    // {
      // draw_heart();
    // }
    // else
    { 
    // update_temp();
      clk->update_clock_face(dsp);
    } 

    if(millis()-last_repaint>TARGET_MS_PER_FRAME){
      dsp->flush_buffer();
      last_repaint = millis();

      // // blink a led 
      // if(blink)
      // {
      //   digitalWrite(LED_BUILTIN, HIGH);
      // }
      // else{
      //   digitalWrite(LED_BUILTIN, LOW);      
      // }
      // blink = !blink;
    }
  }

}

