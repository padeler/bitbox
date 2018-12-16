#include <TimeLib.h>
#include <Wire.h>
#include <DS1307RTC.h>
#include "SD.h"

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
  // if(Serial) handler = new CommHandler(dsp);
  // else handler = NULL;
  
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



// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster.  20 pixels seems a
// good balance.

#define BUFFPIXEL 21

// void bmpDraw(char *filename, Display *dsp) {
//   File     bmpFile;
//   int      bmpWidth, bmpHeight;   // W+H in pixels
//   uint8_t  bmpDepth;              // Bit depth (currently must be 24)
//   uint32_t bmpImageoffset;        // Start of image data in file
//   uint32_t rowSize;               // Not always = bmpWidth; may have padding
//   uint8_t  sdbuffer[BUFFPIXEL]; // pixel in buffer (R+G+B per pixel)
//   boolean  goodBmp = false;       // Set to true on valid header parse
//   boolean  flip    = true;        // BMP is stored bottom-to-top
//   int      w, h, row, col;
//   uint8_t  r, g, b;
//   uint32_t pos = 0, startTime = millis();
  
//   uint8_t x,y;
//   x=0;y=0;

//   Serial.println();
//   Serial.print(F("Loading image '"));
//   Serial.print(filename);
//   Serial.println('\'');
//   // Open requested file on SD card
//   if ((bmpFile = SD.open(filename)) == NULL) {
//     Serial.println(F("File not found"));
//     return;
//   }

//   // Parse BMP header
//   if(read16(bmpFile) == 0x4D42) { // BMP signature
//     Serial.println(F("File size: ")); Serial.println(read32(bmpFile));
//     (void)read32(bmpFile); // Read & ignore creator bytes
//     bmpImageoffset = read32(bmpFile); // Start of image data
//     Serial.print(F("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
//     // Read DIB header
//     Serial.print(F("Header size: ")); Serial.println(read32(bmpFile));
//     bmpWidth  = read32(bmpFile);
//     bmpHeight = read32(bmpFile);
//     if(read16(bmpFile) == 1) { // # planes -- must be '1'
//       bmpDepth = read16(bmpFile); // bits per pixel
//       Serial.print(F("Bit Depth: ")); Serial.println(bmpDepth);
//       if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

//         goodBmp = true; // Supported BMP format -- proceed!
//         Serial.print(F("Image size: "));
//         Serial.print(bmpWidth);
//         Serial.print('x');
//         Serial.println(bmpHeight);

//         // BMP rows are padded (if needed) to 4-byte boundary
//         rowSize = (bmpWidth * 3 + 3) & ~3;

//         // If bmpHeight is negative, image is in top-down order.
//         // This is not canon but has been observed in the wild.
//         if(bmpHeight < 0) {
//           bmpHeight = -bmpHeight;
//           flip      = false;
//         }

//         // Crop area to be loaded
//         w = bmpWidth;
//         h = bmpHeight;
//         if((x+w-1) >= MATRIX_WIDTH)  w = MATRIX_WIDTH  - x;
//         if((y+h-1) >= MATRIX_HEIGHT) h = MATRIX_HEIGHT - y;

//         for (row=0; row<h; row++) { // For each scanline...
//           // Seek to start of scan line.  It might seem labor-
//           // intensive to be doing this on every line, but this
//           // method covers a lot of gritty details like cropping
//           // and scanline padding.  Also, the seek only takes
//           // place if the file position actually needs to change
//           // (avoids a lot of cluster math in SD library).
//           if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
//             pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
//           else     // Bitmap is stored top-to-bottom
//             pos = bmpImageoffset + row * rowSize;
//           if(bmpFile.position() != pos) { // Need seek?
//             bmpFile.seek(pos);
//           }

//           for (col=0; col<w; ) { // For each column...
//             int req_bytes = min((w-col)*3, BUFFPIXEL);
//             int count = bmpFile.read(sdbuffer, req_bytes);
//             if(count<=0 || count%3!=0) // should only read multiple of 3bytes (R-G-B) 
//             { // unexpected EOF
//               Serial.println(F("Unexpected EOF."));
//               return;
//             }

//             for(int p=0;p<count/3;++p)
//             {
//               b = sdbuffer[p*3];
//               g = sdbuffer[p*3+1];
//               r = sdbuffer[p*3+1];
//               dsp->set(col+p, row, CRGB(b,g,r));
//             }

//             col += count/3;
//           } // end pixel
//         } // end scanline

//         Serial.print(F("Loaded in "));
//         Serial.print(millis() - startTime);
//         Serial.println(" ms");
//       } // end goodBmp
//     }
//   }

//   bmpFile.close();
//   if(!goodBmp) Serial.println(F("BMP format not recognized."));

// }

// // These read 16- and 32-bit types from the SD card file.
// // BMP data is stored little-endian, Arduino is little-endian too.
// // May need to reverse subscript order if porting elsewhere.

// uint16_t read16(File f) {
//   uint16_t result;
//   ((uint8_t *)&result)[0] = f.read(); // LSB
//   ((uint8_t *)&result)[1] = f.read(); // MSB
//   return result;
// }

// uint32_t read32(File f) {
//   uint32_t result;
//   ((uint8_t *)&result)[0] = f.read(); // LSB
//   ((uint8_t *)&result)[1] = f.read();
//   ((uint8_t *)&result)[2] = f.read();
//   ((uint8_t *)&result)[3] = f.read(); // MSB
//   return result;
// }



// void draw_heart(){
//   if(millis()-last_upd>100){
//     last_upd = millis();
//     dsp->fillrect(0,0,16,16, CRGB::Black);
//     bmpDraw("/test/minnie2.bmp", dsp);
//     dsp->repaint();
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
  
  // if(handler && handler->isReceiving()){
  //   handler->check_timeout();
  // }
  // else if(Serial.available()==0)
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
    }
  }

}

