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
            for(int p=0;p<MATRIX_WIDTH*MATRIX_HEIGHT;++p)
            {
                CRGB c = dsp->get(p%MATRIX_WIDTH,p/MATRIX_WIDTH);
                if((c[0]|c[1]|c[2])!=0)// there is color in this pixel
                {
                    int ny = p/MATRIX_WIDTH + 1;
                    int nx = p%MATRIX_WIDTH -1 + random8(0,3);
                    
                    if(frame_no<20) ny = min(ny,15);

                    CRGB nc = dsp->get(nx,ny);
                    if((nc[0]|nc[1]|nc[2])==0)
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