# bitbox
RGB Led matrix for clock/weather/info/animations.

Bitbox is a RGB LED matrix controlled using an arduino board. It combines the functions of the [GameFrame](https://ledseq.com/product/game-frame) by LEDSEQ and the [RibbaPi](https://github.com/stahlfabrik/RibbaPi). 

Bitbox can work as a standalone device displaying data/time or controlled over USB to display images send by the host.


This repository contains arduino code and build notes for the bitbox display.

For the s/w on the PC side check the [Pybitbox](https://github.com/padeler/Pybitbox) repository.  

## 3rd party arduino libraries 

- WS2812 control: [FastLED](https://github.com/FastLED/FastLED)
- [Time](https://www.pjrc.com/teensy/td_libs_Time.html)
- [DS1307RTC](https://github.com/PaulStoffregen/DS1307RTC) 

## Useful links

- [Power Topology](https://learn.adafruit.com/1500-neopixel-led-curtain-with-raspberry-pi-fadecandy/power-topology)


## Making-of bitbox 

The 16x16 matrix is made of four 8x8 panels of WS2812 RGB LEDs I got from [hobbyking](https://hobbyking.com/en_us/2812-8-8-64-bit-full-color-5050-rgb-led-module.html).
![matrix0](/photos/matrix0.jpg)
Since I will most probably make changes in the future, I used connectors wherever possible.
![matrix1](/photos/matrix1.jpg)
Testing the leds to make sure all are working ok.
![screen test without diffuser](/photos/mario0.jpg)
In order to get a nice "pixel" effect a diffuser is needed. Here the diffuser is a piece of A4 paper.
![screen test with paper diffuser](/photos/mario1.jpg)
For the final assembly, a 5mm piece of tinted plexiglas and a thinner piece of paper is used. 
![Grid on led matrix](/photos/grid0.jpg)
A [3D printed](/model/diffuser_grid.scad) grid goes between the diffuser and the LED panel. 
This helps create a nice "square" pixel effect.
![diffuser grid and wooden frame](/photos/grid.jpg)
Putting everything together. The wooden frame before painting.
![electronics assembly in the wooden frame](/photos/assemply.jpg)
Testing the clock function. Here the just a paper diffuser is used.
![test with frame and paper diffuser](/photos/test_paper_diffuser.jpg)
More testing with the tinted plexiglas diffuser. The clock set to the "pong" clock-face.
![test with Plexiglas diffuser](/photos/test_tinted_plexiglass.jpg)
![test tinted Plexiglas closeup](/photos/tinted_plexiglass2.jpg)
A 3mm piece of white plexiglas is used for the back cover. 
![back side plexiglas](/photos/back_plexiglass.jpg)
The USB port is used for powering the frame and electronics and for serial connection with the arduino.
![USB port for power and serial comm](/photos/usb_port.jpg)
The final result: Bitbox in clock mode with the "break-out" clock-face.
![final result](/photos/final_result.jpg)

