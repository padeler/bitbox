#include <TimeLib.h>
#include <Wire.h>
#include <DS1307RTC.h>

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

#define TARGET_MS_PER_FRAME 30
#define DEFAULT_TIME 1514589221


/* ***************************************************************** */
/* ************************* MAIN SKETCH *************************** */
/* ***************************************************************** */

CommHandler *handler;

Display *dsp;
Clock *clk;


unsigned long last_repaint = 0;

// uint8_t heart_fr=0;

unsigned long last_upd=0;

// DHT dht(DHTPIN, DHTTYPE);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  // Serial.begin(9600);
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

// void draw_heart(){

//   if(millis()-last_upd>100){
//     uint8_t har[] = {0,1,1,2,2,2,2,1,1,0};
//     last_upd = millis();
//     dsp->fillrect(0,0,16,16, CRGB::Black);
//     int offset = (har[heart_fr%10])*(heart_width*heart_height*3);
//     dsp->drawImage_pm(heart, offset, 0, 0, heart_width, heart_height);    
//     dsp->repaint();
//     heart_fr+=1;
//   }
// }


// uint8_t hum;
// float temp;

// void update_temp(){

//   if(millis()-last_upd>50000){
//     hum = round(dht.readHumidity());
//     temp = dht.readTemperature();
//     last_upd = millis();

//     // Check if any reads failed and exit early (to try again).
//     if (isnan(hum) || isnan(temp)) {
//       Serial.println(F("Failed to read from DHT sensor!"));
//       return;
//     }

//     dsp->animation_push(new Message(String(hum)+" %", 0,-8,0,1, true)); 
//     dsp->animation_push(new Message(String(temp,1), 0,-8,0,1, true)); 
//     dsp->animation_push(new Collapse()); 
//   }
// }

void loop() {
  
  if(handler && handler->isReceiving()){
    handler->check_timeout();
  }
  else if(Serial.available()==0)
  { //only if serial IO is not pending


    // if(day()==28 && month()==4 && hour()>=8)
    // {
    //   draw_heart();
    // }
    // else
    { 
    // update_temp();
      clk->update_clock_face(dsp);
    } 

    if(millis()-last_repaint>TARGET_MS_PER_FRAME){
      dsp->flush_buffer();
      last_repaint = millis();
    }
  }

}

