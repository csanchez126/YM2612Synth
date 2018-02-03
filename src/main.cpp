#include "ChipPinMapping.h"
#include <SPI.h>
#include <SdFat.h>
#include <Wire.h>
#include <Adafruit_SleepyDog.h>

//Timing Variables
float singleSampleWait = 0;
const int sampleRate = 44100; //44100 standard
const float WAIT60TH = ((1000.0 / (sampleRate/(float)735))*1000);
const float WAIT50TH = ((1000.0 / (sampleRate/(float)882))*1000);
uint32_t waitSamples = 0;
unsigned long preCalced8nDelays[16];
unsigned long preCalced7nDelays[16];
unsigned long lastWaitData61 = 0;
unsigned long cachedWaitTime61 = 0;
unsigned long pauseTime = 0;
unsigned long startTime = 0;

//Song Data Variables
#define MAX_PCM_BUFFER_SIZE 102400 //In bytes
uint8_t pcmBuffer[MAX_PCM_BUFFER_SIZE];
uint32_t pcmBufferPosition = 0;
uint8_t cmd;
uint32_t loopOffset = 0;
uint16_t loopCount = 0;
uint16_t nextSongAfterXLoops = 3;

void wait10nS(int multiplier) //10 ns of delay * multiplier. Teensy 3.5 specific value due to clock speed. Adjust as needed.
{
  int i = multiplier;
  while(i --> 0)
    __asm__ volatile ("nop"); //Approx. 10 ns of delay
}

void SendYMByte(byte b)
{
    for(int i=0; i<8; i++)
    {
      digitalWriteFast(YM_DATA[i], ((b >> i)&1));
    }
}

void write_ym(uint8_t data) {
	digitalWriteFast(YM_CS, LOW);
	SendYMByte(data);
	delayMicroseconds(1);
	digitalWriteFast(YM_WR, LOW);// Write data
	delayMicroseconds(5);
	digitalWriteFast(YM_WR, HIGH);
	delayMicroseconds(5);
	digitalWriteFast(YM_CS, HIGH);
}

void setreg(uint8_t reg, uint8_t data) {
	digitalWriteFast(YM_A0, LOW);
	write_ym(reg);
	digitalWriteFast(YM_A0, HIGH);  // A0 high (write register)
	write_ym(data);
}


void SilenceAllChannels()
{
  digitalWriteFast(YM_A0, LOW);
  digitalWriteFast(YM_A1, LOW);
  digitalWriteFast(YM_CS, HIGH);
  digitalWriteFast(YM_WR, HIGH);
  digitalWriteFast(YM_RD, HIGH);
  digitalWriteFast(YM_IC, HIGH);
  delay(1);
  digitalWriteFast(YM_IC, LOW);
  delay(1);
  digitalWriteFast(YM_IC, HIGH);
}

void setup()
{

  for(int i = 0; i<8; i++)
  {
    pinMode(YM_DATA[i], OUTPUT);
  }
  //Sound chip control pins
  pinMode(YM_IC, OUTPUT);
  pinMode(YM_CS, OUTPUT);
  pinMode(YM_WR, OUTPUT);
  pinMode(YM_RD, OUTPUT);
  pinMode(YM_A0, OUTPUT);
  pinMode(YM_A1, OUTPUT);
  Serial.begin(115200);
  SilenceAllChannels();

  /* YM2612 Test code */
  	setreg(0x22, 0x00); // LFO off
  	setreg(0x27, 0x00); // Note off (channel 0)
  	setreg(0x28, 0x01); // Note off (channel 1)
  	setreg(0x28, 0x02); // Note off (channel 2)
  	setreg(0x28, 0x04); // Note off (channel 3)
  	setreg(0x28, 0x05); // Note off (channel 4)
  	setreg(0x28, 0x06); // Note off (channel 5)
  	setreg(0x2B, 0x00); // DAC off
  	setreg(0x30, 0x71); //
  	setreg(0x34, 0x0D); //
  	setreg(0x38, 0x33); //
  	setreg(0x3C, 0x01); // DT1/MUL
  	setreg(0x40, 0x23); //
  	setreg(0x44, 0x2D); //
  	setreg(0x48, 0x26); //
  	setreg(0x4C, 0x00); // Total level
  	setreg(0x50, 0x5F); //
  	setreg(0x54, 0x99); //
  	setreg(0x58, 0x5F); //
  	setreg(0x5C, 0x94); // RS/AR
  	setreg(0x60, 0x05); //
  	setreg(0x64, 0x05); //
  	setreg(0x68, 0x05); //
  	setreg(0x6C, 0x07); // AM/D1R
  	setreg(0x70, 0x02); //
  	setreg(0x74, 0x02); //
  	setreg(0x78, 0x02); //
  	setreg(0x7C, 0x02); // D2R
  	setreg(0x80, 0x11); //
  	setreg(0x84, 0x11); //
  	setreg(0x88, 0x11); //
  	setreg(0x8C, 0xA6); // D1L/RR
  	setreg(0x90, 0x00); //
  	setreg(0x94, 0x00); //
  	setreg(0x98, 0x00); //
  	setreg(0x9C, 0x00); // Proprietary
  	setreg(0xB0, 0x32); // Feedback/algorithm
  	setreg(0xB4, 0xC0); // Both speakers on
  	setreg(0x28, 0x00); // Key off
  	setreg(0xA4, 0x29);	//
  	setreg(0xA0, 0x60); // Set frequency
}

void loop()
{
    byte b[] = {0x60,0x70,0x80,0x95,0xA9};
  	/* Program loop */
  	for(int i=0;i<5;i++) {
      setreg(0xA4, 0x24);
      setreg(0xA0, 0x8C);
      setreg(0x28, 0xF0); // Key on
  		delay(150);
  		setreg(0x28, 0x00); // Key off

      setreg(0xA4, 0x23);
      setreg(0xA0, 0x68);
      setreg(0x28, 0xF0); // Key on
  		delay(150);
  		setreg(0x28, 0x00); // Key off

      setreg(0xA4, 0x22);
      setreg(0xA0, 0xDD);
      setreg(0x28, 0xF0); // Key on
  		delay(150);
  		setreg(0x28, 0x00); // Key off

      setreg(0xA4, 0x1C);
      setreg(0xA0, 0x8C);
      setreg(0x28, 0xF0); // Key on
  		delay(150);
  		setreg(0x28, 0x00); // Key off
  		delay(500);
  	}
}
// 48C a#
// 44B a
// 40D g#
// 3D3 g
// 368 f
// 337 e
// 309 d#
// 2DD d
// 2B4 c#
// 28D c
// 269 b
