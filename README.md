# bitbox
RGB Led matrix for clock/weather/info/animations.

Bitbox is a RGB LED matrix controled using an arduino board. It compines the functions of the [GameFrame](https://ledseq.com/product/game-frame) by LEDSEQ and the [RibbaPi](https://github.com/stahlfabrik/RibbaPi). 

Bitbox can work as a standalone devide displaying data/time or controled over USB to display images send by the host.


This repository contains arduino code and build notes for the bitbox display.

For the s/w on the PC side check the [Pybitbox](https://github.com/padeler/Pybitbox) repository.  

## 3rd party arduino libraries 

- WS2812 control: [FastLED](https://github.com/FastLED/FastLED)
- [Time](https://www.pjrc.com/teensy/td_libs_Time.html)

## Usefull links

- [Power Topology](https://learn.adafruit.com/1500-neopixel-led-curtain-with-raspberry-pi-fadecandy/power-topology)


## TODOs

- Fusebox
- Separate power supply for arduino and LED panel
- Diodes
