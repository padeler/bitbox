#include "FastLED.h"



void flash(int times){
  digitalWrite(LED_BUILTIN, LOW);
  delay(200);  
  int total=200;
  
  int del = total/times;
  for(int i=0;i<times;++i){
    delay(del);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(del);
    digitalWrite(LED_BUILTIN, LOW);      
  }
}


inline String format_digit(int d){
  String s(d);
  if(d < 10)
    return String("0"+s);
  return s;
}


String format_time(){
  return format_digit(hour()) + format_digit(minute());
}


void update_time(byte *buf, int buf_len){
  // read pc time 
  unsigned long t = atol(buf);
  setTime(t);
    
//  String b = (char*)buf;
//  int Y = b.substring(0,4).toInt();
//  int M = b.substring(5,7).toInt();
//  int D = b.substring(8,10).toInt();
//
//  int hh = b.substring(11,13).toInt();
//  int mm = b.substring(14,16).toInt();
//  int ss = b.substring(17,19).toInt();
//
//  setTime(hh,mm,ss,D,M,Y);  
//  
//  String newTime = String(hh)+":"+String(mm)+":"+String(ss) +" "+String(D)+"-"+String(M)+"-"+String(Y) ; 
//  Serial.println("GOT TIME: "+newTime);
}


/*
 * SETTINGS BUF:
 * 
 * Brightness
 * BNNN RFU
 */
void update_settings(byte *buf, int buf_len){

  String settings = (char*)buf;
  int brightness = settings.substring(1,4).toInt();
  FastLED.setBrightness(brightness);
//  Serial.println("Brightness to "+String(brightness));

}

