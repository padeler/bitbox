/**
 * The Message class implements a scrolling text animation optimized for the bitbox 16x16 display.
 */

#include "display.h"
#include "font.h"

#define SCROLL_SPEED 100 

class Message: public Animation
{
public:
    Message(const String &txt, int x, int y, int8_t dx, int8_t dy):dx(dx),dy(dy),x(x),y(y),txt(txt)
    {
        fg_color = CRGB::White;
        compact = false;
        last_repaint = 0;
    }

    bool update(Display *dsp)
    {
        if(millis()-last_repaint>SCROLL_SPEED)
        {
            dsp->fillrect(0,y,MATRIX_WIDTH, char_height, CRGB::Black);
            
            x+=dx;y+=dy;

            dsp->draw_string(txt, x, y, fg_color, compact, false);
            dsp->repaint();

            last_repaint = millis();

            if((dx<0 && x<-((int)(txt.length()*char_width))) ||
               (dx>0 && x> MATRIX_WIDTH) || 
               (dy<0 && y<-char_height) ||
               (dy>0 && y> MATRIX_HEIGHT))
            { // text scrolled completelly off screen. Animation done.
                return true;
            }
        }
        return false;
    }

private:
    unsigned long last_repaint;
    bool compact;
    int8_t dx, dy;
    int x,y;
    CRGB fg_color;
    String txt;
};