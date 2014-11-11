#include <Arduino.h>
#include "config.h"

#include "PPMTX.h"

#define PPMTX_IDLE_TIME 1500
#define PPMTX_CHANNEL_TIME 2500
#define PPMTX_FRAME_TIME (8 * PPMTX_CHANNEL_TIME)

#define PPMTX_HIGH_DELAY 10
#define PPMTX_LOW_DELAY 11
#define PPMTX_FRAME_DELAY 0

// ================================ Public ================================ //

void PPMTX::init()
{
  // TX pin configuration
  digitalWrite(PPMTX_PIN, LOW);
  pinMode(PPMTX_PIN, OUTPUT);
}

void PPMTX::sendFrame()
{
  uint32_t waitingTime = nextFrameTime - micros();
  if(waitingTime > 0) delayMicroseconds(waitingTime);
  nextFrameTime = micros() + PPMTX_FRAME_TIME - PPMTX_FRAME_DELAY;

  for(int i=0; i<PPMTX_CHANNELS; i++)
  {
    int16_t time = channels[i] << 2;
    setPin(HIGH);
    delayMicroseconds(PPMTX_IDLE_TIME + time - PPMTX_HIGH_DELAY);
    setPin(LOW);
    delayMicroseconds((PPMTX_CHANNEL_TIME-PPMTX_IDLE_TIME) - time - PPMTX_LOW_DELAY);
  }
}

void PPMTX::setChannel(uint8_t id, int8_t value)
{
  if(id < PPMTX_CHANNELS)
  {
    channels[id] = value;
  }
}

// ================================ Private ================================ //

uint32_t PPMTX::nextFrameTime = 0;

int8_t PPMTX::channels[PPMTX_CHANNELS];

void PPMTX::setPin(uint8_t state)
{
  switch(state)
  {
  case HIGH:
    pinMode(PPMTX_PIN, INPUT); // high impedance
    break;
  case LOW:
    pinMode(PPMTX_PIN, OUTPUT); // 0V
    break;
  }
}

