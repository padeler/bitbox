# bitbox
RGB Led matrix for clock/weather/info/animations.

Bitbox is a RGB LED matrix controled using an arduino board. It compines the functions of the [GameFrame](https://ledseq.com/product/game-frame) by LEDSEQ and the [RibbaPi](https://github.com/stahlfabrik/RibbaPi). 

Bitbox can work as a standalone devide displaying data/time or controled over USB to display images send by the host.


This repository contains arduino code and build notes for the bitbox display.

For the s/w on the PC side check the [Pybitbox](https://github.com/padeler/Pybitbox) repository.  

## 3rd party arduino libraries 

- WS2812 control: [FastLED](https://github.com/FastLED/FastLED)
- [Time](https://www.pjrc.com/teensy/td_libs_Time.html)
- [DS1307RTC](https://github.com/PaulStoffregen/DS1307RTC) 

## Usefull links

- [Power Topology](https://learn.adafruit.com/1500-neopixel-led-curtain-with-raspberry-pi-fadecandy/power-topology)


## Photos 

![matrix0](/photos/matrix0.jpg)
![matrix1](/photos/matrix1.jpg)
![screen test without diffuser](/photos/mario0.jpg)
![screen test with paper diffuset](/photos/mario1.jpg)
![Grid on len matrix](/photos/grid0.jpg)
![diffuser grid and wooden frame](/photos/grid.jpg)
![electronics assemply in the wooden frame](/photos/assemply.jpg)
![test with frame and paper diffuser](/photos/test_paper_diffuser.jpg)
![test with plexiglass diffuser](/photos/test_tinted_plexiglass.jpg)
![test tinted plexiglass closup](/photos/tinted_plexiglass2.jpg)
![back side plexiglass](/photos/back_plexiglass.jpg)
![usb port for power and serial comm](/photos/usb_port.jpg)
![final result](/photos/final_result.jpg)

