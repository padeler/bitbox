#include "clock.h"

inline String format_digit(int d){
  String s(d);
  if(d < 10)
    return String("0"+s);
  return s;
}

void Clock::draw(){

  if(cfg->clock_mode & CLOCK_MODE_RANDOM && ((millis() - last_bg_change)> cfg->clock_change_bg))
  {
    reset_clock_face();
    last_bg_change = millis();
    uint8_t new_mode;
    do{
       new_mode = (uint8_t)random(CLOCK_MODE_PLAIN) | CLOCK_MODE_RANDOM;
    }while(new_mode==cfg->clock_mode);

    cfg->clock_mode = new_mode;
  }
  
  draw_clock_face();
  dsp->repaint();
}

void Clock::draw_time(const Point &h_pos, const Point &m_pos, const CRGB &h_color, const CRGB &m_color, bool clear_before_draw, bool compact, bool overlay, bool force_redraw){
  int nhh = hour();
  int nmm = minute();

  if(nhh!=hh || nmm!=mm || force_redraw)
  { // only draw if needed.
    String hstr,mstr;
    if(clear_before_draw){
      hstr = format_digit(hh);
      mstr = format_digit(mm);
      dsp->draw_string(hstr, h_pos.x, h_pos.y, CRGB::Black, compact, false);
      dsp->draw_string(mstr, m_pos.x, m_pos.y, CRGB::Black, compact, false);
    }
    hh = nhh;
    mm = nmm;
    hstr = format_digit(nhh);
    mstr = format_digit(nmm);
    dsp->draw_string(hstr, h_pos.x, h_pos.y, h_color, compact, overlay);
    dsp->draw_string(mstr, m_pos.x, m_pos.y, m_color, compact, overlay);
  }
}

