#include <Arduino.h>
#include "config.h"

#include "ESC.h"

void ESC::setChannel(uint8_t channelId, uint16_t time)
{
  if(channelId < ESC_CHANNELS)
  {
    channels[channelId] = time;
  }
}

uint16_t ESC::getChannel(uint8_t channelId)
{
  if(channelId < ESC_CHANNELS)
  {
    return channels[channelId];
  }
  else { return 0; }
}

uint16_t ESC::channels[] = {};
