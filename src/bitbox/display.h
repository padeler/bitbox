
#include "FastLED.h"

#define NUM_LEDS 256
#define DATA_PIN 5

#define MATRIX_WIDTH 16
#define MATRIX_HEIGHT 16

void init_display();

void update_led(int pos, char val);

void update_leds(byte *buf, int offset, int buf_size);

inline int find_led_index(uint8_t row, uint8_t col);

void draw_chr(int x, int y, CRGB color, bool compact, char c);
void draw_string(int x, int y, CRGB color, bool compact, String str);

void set_led(int x, int y,  CRGB color);

void fillrect(int x, int y, int width, int height, CRGB color);


inline void clear_display(){
  fillrect(0, 0, MATRIX_WIDTH, MATRIX_HEIGHT, CRGB(0,0,0));
}

inline void repaint(){
  FastLED.show();
}


