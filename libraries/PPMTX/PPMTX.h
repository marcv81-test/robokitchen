#ifndef PPMTX_H
#define PPMTX_H

#include <Arduino.h>
#include "config.h"

// Check the TX pin
#if !defined PPMTX_PIN
  #error PPMTX_PIN must be defined
#endif

// Check the number of channels
#if !defined PPMTX_CHANNELS || PPMTX_CHANNELS < 1 || PPMTX_CHANNELS > 7
  #error PPMTX_CHANNELS must be between 1 and 7
#endif

/*!
 * Blocking implementation of a PPM signal generator. The output is 0V or
 * high impendance hence requires a pull-up resistor.
 */

class PPMTX
{
  public:

    static void init();
    static void sendFrame();
    static void setChannel(uint8_t id, int8_t value);

  private:
    static uint32_t nextFrameTime;
    static int8_t channels[PPMTX_CHANNELS];
    static void setPin(uint8_t state);
};

#endif // PPMTX_H

