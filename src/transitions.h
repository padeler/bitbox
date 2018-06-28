#pragma once
#include "display.h"

#define TR_MS_PER_FRAME 60

class Melt: public Animation
{
public:
    Melt()
    {
        last_repaint = 0;
        frame_no = 0;
    }
    bool update(Display *dsp)
    {
        if(millis()-last_repaint>TR_MS_PER_FRAME)
        {
            for(int8_t y=MATRIX_HEIGHT-2;y>=0;y--)
            {
                for(int8_t x=0;x<MATRIX_WIDTH;x++)
                {
                    dsp->set(x,y+1,dsp->get(x,y));
                }
            }

            dsp->fadetoblack(0,0, MATRIX_WIDTH, MATRIX_HEIGHT, 220);
            dsp->repaint();
            last_repaint=millis();

            frame_no++;
        }

        if(frame_no==16) return true; // animation finished
        return false;
    }


    uint8_t frame_no;
    unsigned long last_repaint;
};


class Collapse: public Animation
{
public:
    Collapse()
    {
        last_repaint = 0;
        frame_no = 0;
    }
    bool update(Display *dsp)
    {

        if(millis()-last_repaint>TR_MS_PER_FRAME)
        {
            for(int p=MATRIX_WIDTH*MATRIX_HEIGHT-1;p>=0;--p)
            {
                CRGB c = dsp->get(p%MATRIX_WIDTH, p/MATRIX_WIDTH);
                if(IS_ON(c))// there is color in this pixel
                {
                    int ny = p/MATRIX_WIDTH + 1;
                    int nx = p%MATRIX_WIDTH;
                    if(nx<8) nx-= random(-1,3);
                    else nx+= random(-1,3);
                    
                    if(frame_no<20){ 
                        ny = min(ny,15);
                        nx = max(0,nx);
                        nx = min(nx,15);
                    }

                    CRGB nc = dsp->get(nx,ny);
                    if(!IS_ON(nc))
                    {
                        dsp->set(p%MATRIX_WIDTH,p/MATRIX_WIDTH,CRGB::Black);
                        dsp->set(nx,ny,c);
                    }
                }
            }
            
            dsp->repaint();
            last_repaint=millis();

            frame_no++;
        }

        if(frame_no==40) return true; // animation finished
        return false;
    }


    uint8_t frame_no;
    unsigned long last_repaint;
};