#pragma once 

#include "FastLED.h"

#define NUM_LEDS 256
#define DATA_PIN 5

#define MATRIX_WIDTH 16
#define MATRIX_HEIGHT 16

#define MAX_BRIGHTNESS 30 
#define DEFAULT_BRIGHTNESS 10

#define IS_ON(c) ((c[0]|c[1]|c[2])!=0)
class Animation;

class Display{
public:
  Display();

  void update_led(int pos, char val);
  void update_leds(byte *buf, int offset, int buf_size);
  
  inline int find_index(uint8_t row, uint8_t col);
  
  void draw_char(char c, int x, int y, CRGB color, bool compact, bool overlay);
  void draw_string(String str, int x, int y, CRGB color, bool compact, bool overlay);

  CRGB get(int x, int y);
  void set(int x, int y,  CRGB color);
  void overlay(int x, int y, CRGB color);

  void fillrect(int x, int y, int width, int height, CRGB color);
  void drawImage_pm(const byte *buf, int offset, int8_t x, int8_t y, uint8_t width, uint8_t height);

  void fadetoblack(int x, int y, int width, int height, uint8_t amount);
  
  inline void clear_display(){
    fillrect(0, 0, MATRIX_WIDTH, MATRIX_HEIGHT, CRGB(0,0,0));
  }

  inline void set_brightness(uint8_t br)
  {
    if(br<MAX_BRIGHTNESS){
      FastLED.setBrightness(br);    
    }
  }

  /* Animation stack methods */ 
  Animation* animation_pop();
  bool animation_delete(Animation *anim);
  void animation_push(Animation *new_anim);
  uint8_t animation_stack_size();

  /* ********************** */

  void flush_buffer();

  
  inline void repaint(){
    repaint_needed = true;
  }

  inline bool is_animations_enabled(){
    return animations_enabled;
  }

  inline void set_animations_enabled(bool v){
    animations_enabled = v;
  }
  
private:
  
  bool repaint_needed;
  bool animations_enabled;
  bool force_redraw;
  
  // Define the array of leds
  CRGB leds[NUM_LEDS];

  Animation *animation; // the top of the animation list
};


class Animation{
  
public:

  Animation():next(NULL)
  {}

  virtual ~Animation(){}
  /**
   * Update animation (draw next frame on dsp)
   * returns: True if the animation has more frames, False if it is finished
   */
  virtual bool update(Display *dsp, bool force_repaint);

  Animation *next;
};