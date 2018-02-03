# YM2612 based synthesizer

Using the Sega Genesis sound chip, I will attempt to build a midi controller synthesizer, driven by the Teensy 3.5 prototyping platform.
I will be using the following code bases and documentation to guide my build:

https://github.com/AidanHockey5/Teensy_3_5_VGM_Player_YM2612_SN76489
https://github.com/skywodd/avr_vgm_player
https://github.com/endofexclusive/ym2612_midi
http://www.smspower.org/maxim/Documents/YM2612

# Hook-up Guide (from AidanHockey5's repo)

This set of hook-up tables is based on the pin numbers listed in ChipPinMapping.h. You may change any of these pins to suit your build inside of ChipPinMapping.h, but it is not recommended to change any SPI or I2C data bus pins.

Teensy 3.5 | YM2612
------------ | -------------
0-7 | D0-D7
38  | IC
39 | CS (previously 37)
36 | WR
35 | RD
34 | A0
33 | A1
