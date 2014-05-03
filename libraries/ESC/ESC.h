#ifndef ESC_H
#define ESC_H

#include <Arduino.h>
#include "config.h"

#define ESC_CHANNELS 4

/*!
 * This class provides routines to drive ESCs using PWMs with a 490kHz refresh rate.
 * There are exactly 4 channels associated to the pins 9, 10, 11, and 3. The channel
 * high-level time shall be between 0 and 2000 microseconds. The accuracy is 8ms.
 */
class ESC
{

  public:

    /*!
     * Set the PWM cycle of a channel.
     *
     * @param channelId ID of the channel to set
     * @param time High level time in microseconds
     */
    static void setChannel(uint8_t channelId, uint16_t time);

    /*! Initialise the library. */
    static void init();

};

#endif // ESC_H
