#ifndef ESC_H
#define ESC_H

#include <Arduino.h>
#include "config.h"

#define ESC_CHANNELS 4

class ESC
{
  public:
    static void setChannel(uint8_t channelId, uint16_t time);
    static uint16_t getChannel(uint8_t channelId);

  private:
    static uint16_t channels[ESC_CHANNELS];
};

#endif // ESC_H
