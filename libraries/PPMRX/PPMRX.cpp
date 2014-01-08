#include <Arduino.h>
#include "config.h"

#include "PPMRX.h"

// ================================ Public ================================ //

int8_t PPMRX::getChannel(uint8_t channelId)
{
  if(channelId >= PPMRX_CHANNELS) { return PPMRX_ERROR; }
  else { return channels[channelId]; }
}

uint8_t PPMRX::refresh()
{
  uint32_t time = micros();
  uint8_t readFrameId = frameId; // Cache frameId which is volatile
  readFrameId = (readFrameId == 0) ? 2 : readFrameId - 1; // Read the previous frame

  // Outdated frames
  if((time - frame[readFrameId].startTime) > 50000)
  {
    #ifdef PPMRX_DEBUG
      Serial.print("PPMRX: Outdated frame (");
      Serial.print(time - frame[readFrameId].startTime);
      Serial.println(")");
    #endif
    return 0;
  }

  // Incomplete frames
  if(frame[readFrameId].complete == 0)
  {
    #ifdef PPMRX_DEBUG
      Serial.println("PPMRX: Incomplete frame");
    #endif
    return 0;
  }

  // Invalid frames (unexpected number of channels)
  if(frame[readFrameId].channelId != PPMRX_CHANNELS)
  {
    #ifdef PPMRX_DEBUG
      Serial.print("PPMRX: Invalid frame (");
      Serial.print(frame[readFrameId].channelId);
      Serial.println(")");
    #endif
    return 0;
  }

  // Valid frames
  for(uint8_t i=0; i<PPMRX_CHANNELS; i++)
  {
    uint32_t duration = frame[readFrameId].duration[i];
    if((duration > 2000) || (duration < 1000))
    {
      #ifdef PPMRX_DEBUG
        Serial.print("PPMRX: Invalid channel duration (");
        Serial.print(duration);
        Serial.println(")");
      #endif
      channels[i] = PPMRX_ERROR;
    }
    else
    {
      channels[i] = (duration - 1500) >> 2; // Convert to [-125, 125] range
    }
  }
  #ifdef PPMRX_DEBUG
    Serial.print("PPMRX: Valid frame (");
    for(uint8_t i=0; i<PPMRX_CHANNELS; i++)
    {
      Serial.print(frame[readFrameId].duration[i]);
      Serial.print(", ");
    }
    Serial.print(time - frame[readFrameId].startTime);
    Serial.println(")");
  #endif
  return 1;
}

void PPMRX::init()
{
  resetChannels();

  // Reset the frames
  for(uint8_t i=0; i<3; i++)
  {
    frame[i].complete = 0;
    frame[i].channelId = 0;
    frame[i].startTime = 0;
  }

  // Setup the interrupt
  pinMode(PPMRX_PIN, INPUT);
  attachInterrupt(PPMRX_INTERRUPT, PPMRX::interrupt, CHANGE);
}

// ================================ Private ================================ //

volatile uint32_t PPMRX::risingTime = 0;
volatile uint32_t PPMRX::fallingTime = 0;

volatile uint8_t PPMRX::frameId = 0;
volatile PPMRX::frame_t PPMRX::frame[3];

int8_t PPMRX::channels[PPMRX_CHANNELS];

void PPMRX::resetChannels()
{
  for(uint8_t i=0; i<PPMRX_CHANNELS; i++)
  {
    channels[i] = PPMRX_ERROR;
  }
}

void PPMRX::interrupt()
{
  switch(digitalRead(PPMRX_PIN))
  {
    // Rising transitions
    case HIGH:
      risingTime = micros();

      // Complete the frame if the last blank was more than 2ms long
      if((risingTime - fallingTime) > 2000)
      {
        frame[frameId].complete = 1; // Mark frame as complete
        frameId = (frameId == 2) ? 0 : frameId + 1; // Move to next frame
        frame[frameId].complete = 0; // Mark frame as incomplete
        frame[frameId].channelId = 0; // Reset channel
        frame[frameId].startTime = risingTime; // Store frame starting time
      }
    break;

    // Falling transitions
    case LOW:
      fallingTime = micros();

      // If reading a valid channel
      if(frame[frameId].channelId < PPMRX_CHANNELS)
      {
        // Store the channel high level pulse duration
        frame[frameId].duration[frame[frameId].channelId] = fallingTime - risingTime;
      }
      // Move to the next channel
      frame[frameId].channelId++;
    break;
  }
}
