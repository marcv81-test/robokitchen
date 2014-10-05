#include <Arduino.h>
#include "config.h"

#include "ESC.h"

#ifndef cbi
  #define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
  #define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void ESC::setChannel(uint8_t channelId, uint16_t time)
{
  if(channelId < ESC_CHANNELS)
  {
    uint8_t outputCompare = time >> 3;

    switch(channelId)
    {
      case 0: // Pin 9
        OCR1AL = outputCompare;
      break;

      case 1: // Pin 10
        OCR1BL = outputCompare;
      break;

      case 2: // Pin 11
        OCR2A = outputCompare;
      break;

      case 3: // Pin 3
        OCR2B = outputCompare;
      break;
    }
  }
}

void ESC::init()
{
  // --- Timer 1 ---

  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

  OCR1A = 0;
  OCR1B = 0;

  // Table 16-4: Waveform Generation Mode (0001)
  cbi(TCCR1B, WGM13);
  cbi(TCCR1B, WGM12);
  cbi(TCCR1A, WGM11);
  sbi(TCCR1A, WGM10);

  // Table 16-5: Clock Select (011, prescaler = 64)
  cbi(TCCR1B, CS12);
  sbi(TCCR1B, CS11);
  sbi(TCCR1B, CS10);

  // Table 16-3: Compare Output Mode, Phase Correct PWM, pin 9
  sbi(TCCR1A, COM1A1);
  cbi(TCCR1A, COM1A0);

  // Table 16-3: Compare Output Mode, Phase Correct PWM, pin 10
  sbi(TCCR1A, COM1B1);
  cbi(TCCR1A, COM1B0);

  // --- Timer 2 ---

  pinMode(11, OUTPUT);
  pinMode(3, OUTPUT);

  OCR2A = 0;
  OCR2B = 0;

  // Table 18-8: Waveform Generation Mode (001)
  cbi(TCCR2B, WGM22);
  cbi(TCCR2A, WGM21);
  sbi(TCCR2A, WGM20);

  // Table 18-9: Clock Select (10, prescaler = 64)
  sbi(TCCR2B, CS22);
  cbi(TCCR2B, CS21);
  cbi(TCCR2B, CS20);

  // Table 18-7: Compare Output Mode, Phase Correct PWM, pin 11
  sbi(TCCR2A, COM2A1);
  cbi(TCCR2A, COM2A0);

  // Table 18-7: Compare Output Mode, Phase Correct PWM, pin 3
  sbi(TCCR2A, COM2B1);
  cbi(TCCR2A, COM2B0);

  // Calibrate the ESCs (max)
  for(int i=0 ; i<ESC_CHANNELS ; i++) ESC::setChannel(i, MOTOR_MAX);
  delay(3000);

  // Calibrate the ESCs (min)
  for(int i=0 ; i<ESC_CHANNELS ; i++) ESC::setChannel(i, MOTOR_STALL);
  delay(2000);
}

void ESC::stop()
{
  for(int i=0 ; i<ESC_CHANNELS ; i++) ESC::setChannel(i, MOTOR_OFF);
}
