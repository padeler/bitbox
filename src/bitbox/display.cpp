#include "display.h"
#include "font.h"


Display::Display(Config *cfg){
  this->cfg = cfg;
  
  // FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalSMD5050 );
  // FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
  // FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  // update settings using config
  reconfigure();
  
  // clear screen
  clear_display();
  repaint();
}

void Display::draw_string(String str, int x, int y,  CRGB color, bool compact, bool overlay)
{
  int char_step = char_width;
  if(compact)
  {
    char_step--; 
  }
  
  for(int p=0; p<str.length(); ++p)
  {
    char c = str.charAt(p);
    int cx = x+p*char_step;
    draw_char(c, cx, y, color, compact, overlay);

    if(cx>=MATRIX_WIDTH) break;
  }
}

/**
 * Using the font.h character set 
 * Draw the character c. Top left corner of the character is x,y.
 */
void Display::draw_char(char c, int top_left_x, int top_left_y, CRGB color, bool compact, bool overlay){
  int idx = ((int)c - ascii_first);

  uint8_t maxW = char_width;
  uint8_t maxH = char_height;
  if(compact){
    maxW -= 1; maxH -= 2;
  }
  
  if(idx<0 || idx>=charset_size) return; // invalid character

  idx *= char_height; // go to font index 
   
  for(int cy=0; cy<maxH;++cy)
  {
    byte chr_row = pgm_read_byte_near(charset + idx + cy);
    uint8_t mask = 1 << 7;
    for(int cx=0;cx<maxW;++cx)
    {
      if((mask & chr_row)>0) // light led
      {
        if(overlay) overlay_led(top_left_x+cx, top_left_y+cy, color);
        else set_led(top_left_x+cx, top_left_y+cy, color);
      }
      mask >>= 1;
    }
  }
}

void Display::fillrect(int x, int y, int width, int height, CRGB color)
{
  for(int col=x;col<width;++col)
  {
    for(int row=y;row<height;++row)
    {
      set_led(col,row,color);
    }
  }
}


void Display::fadetoblack(int x, int y, int width, int height, uint8_t amount)
{
  for(int col=x;col<width;++col)
  {
    for(int row=y;row<height;++row)
    {
      if(x>=0 && x<MATRIX_WIDTH && y>=0 && y<MATRIX_HEIGHT)
      {
        int frm_idx = find_led_index(row, col);
        leds[frm_idx].nscale8(amount);
      }
    }
  }  
}

void Display::overlay_led(int x, int y, CRGB color){
  if(x>=0 && x<MATRIX_WIDTH && y>=0 && y<MATRIX_HEIGHT)
  {
    int frm_idx = find_led_index(y, x);
    leds[frm_idx] += color;
  }
}
void Display::set_led(int x, int y, CRGB color){
  if(x>=0 && x<MATRIX_WIDTH && y>=0 && y<MATRIX_HEIGHT)
  {
    int frm_idx = find_led_index(y, x);
    leds[frm_idx] = color;
  }
}

void Display::update_led(int pos, char val){
  
  int img_idx = pos/3;
  int color = pos%3;
  
  int row = img_idx/MATRIX_WIDTH;
  int col = img_idx%MATRIX_WIDTH;
  int frm_idx = find_led_index(row, col);
  leds[frm_idx][color] = val;
}

void Display::update_leds(byte *buf, int offset, int buf_size)
{ 
  for(int buf_idx=0;buf_idx<buf_size;buf_idx+=3)
  {
    int img_idx = (offset+buf_idx)/3;

    int row = img_idx/MATRIX_WIDTH;
    int col = img_idx%MATRIX_WIDTH;
    int frm_idx = find_led_index(row, col);

    leds[frm_idx][0] = buf[buf_idx];
    leds[frm_idx][1] = buf[buf_idx+1];
    leds[frm_idx][2] = buf[buf_idx+2];
  }
}

inline int Display::find_led_index(uint8_t row, uint8_t col){
  int frm_idx;
  if(col<8){ // FIXME convention specific to the wiring of my led matrix: Two parts of 16 rows by 8 columns connected in series
     frm_idx = row*8+col; 
  }
  else{
    frm_idx = MATRIX_HEIGHT*8 + row*8 + (col-8);
  }
  return frm_idx;  
}

