#ifndef PPMRX_H
#define PPMRX_H

#include <Arduino.h>
#include "config.h"

// Check the PPM pin and define the associated interrupt
#if !defined PPMRX_PIN || (PPMRX_PIN != 2 && PPMRX_PIN != 3)
  #error PPMRX_PIN must be 2 or 3
#endif
#if(PPMRX_PIN == 2)
  #define PPMRX_INTERRUPT 0
#endif
#if(PPMRX_PIN == 3)
  #define PPMRX_INTERRUPT 1
#endif

// Check the number of channels
#if !defined PPMRX_CHANNELS || PPMRX_CHANNELS < 1 || PPMRX_CHANNELS > 7
  #error PPMRX_CHANNELS must be between 1 and 7
#endif

// Channel value after read error
#define PPMRX_ERROR 0x80

/*!
 * This class provides functions to decode a PPM signal. The signal is divided into
 * 20ms long frames. Each frame contains eight 2.5ms channels. Each channel is set to
 * the high level for 1ms to 2ms. The length of this high level pulse determines the
 * value of the channel. The last channel is always left empty for synchronisation.
 */
class PPMRX
{
  public:

    /*! Get a channel value, from -125 to 125. */
    static int8_t getChannel(uint8_t channelId);

    /*! Refresh the channels values. */
    static uint8_t refresh();

    /*! Initialise the PPM decoder. */
    static void init();

  private:

    /*! This type stores a PPM frame. */
    typedef struct
    {
      uint32_t duration[PPMRX_CHANNELS]; // High level pulses durations in microseconds
      uint32_t startTime; // Start time of the frame
      uint8_t channelId; // Active channel
      uint8_t complete; // Whether or not the frame is complete yet
    } frame_t;

    /*! Last rising time of the PPM signal. */
    static volatile uint32_t risingTime;

    /*! Last falling time of the PPM signal. */
    static volatile uint32_t fallingTime;

    /*!
     * We alternate between three frames. When a frame is updated with the PPM signal,
     * the previous one provides the latest data. The extra frame is a safety buffer
     * in case a new frame begins while reading the latest data.
     */
    static volatile frame_t frame[3];

    /*! Active frame. */
    static volatile uint8_t frameId;

    /*! Latest channels values. */
    static int8_t channels[PPMRX_CHANNELS];

    /*! Reset the channels values. */
    static inline void resetChannels();

    /*! PPM decoder ISR. */
    static void interrupt();
};

#endif // PPMRX_H
