#ifndef SERVO_H
#define SERVO_H

#include <Arduino.h>
#include "config.h"

#if !defined SERVO_CHANNELS || SERVO_CHANNELS < 1 || SERVO_CHANNELS > 7
  #error SERVO_CHANNELS must be between 1 and 7
#endif

#if !defined SERVO_HIGH_LEVEL_MIN_TIME
  #define SERVO_HIGH_LEVEL_MIN_TIME 1000
#endif
#if !defined SERVO_HIGH_LEVEL_MAX_TIME
  #define SERVO_HIGH_LEVEL_MAX_TIME 2000
#endif

#define SERVO_CYCLE_TOTAL_TIME 2500
#define SERVO_FRAME_TOTAL_TIME 20000
#define SERVO_FRAME_SYNC_TIME \
  (SERVO_FRAME_TOTAL_TIME - (SERVO_CYCLE_TOTAL_TIME * SERVO_CHANNELS))

#define SERVO_DELAY 13

/*!
 * This class provides the timing routines used to drive servos. It uses the timer 2
 * interrupts in order to allow other operations to happen in parallel.
 */
class ServoTimer
{
  public:

    /*! This type stores a duration in a timer-friendly format. */
    typedef struct
    {
      uint8_t primary;
      uint8_t secondary;
    } duration_t;

    /*!
     * Reset the timer and program a new duration. This function shall only be called
     * within the timer/servo ISR. The timer/servo ISR shall always call this function
     * before returning.
     */
    static void reset(duration_t duration);

    /*! Create a timer-friendly duration from microseconds. */
    static duration_t createDuration(uint16_t time);

    /*! Initialise the timer. */
    static void init();

    /*! Timer ISR. Calls the servo ISR after the programmed duration has elapsed. */
    static inline void interrupt();

  private:

    /*! Timer ticks counter. */
    static uint8_t counter;

    /*! Duration currently programmed. */
    static duration_t duration;
};

/*!
 * This class provides routines to drive servos. The number of channels (up to 7)
 * is defined in SERVO_CHANNELS and their associated pins in SERVO_PINS. Each frame
 * lasts 20ms (50Hz) and successively transmits a PWM to each channel.
 */
class Servo
{
  public:

    /*!
     * Set the PWM cycle of a channel.
     *
     * @param channelId ID of the channel to set
     * @param time High level time in microseconds
     */
    static void setChannel(uint8_t channelId, uint16_t time);

    /*! Initialise the servos. */
    static void init();

    /*! Servo ISR. Called back from timer ISR. */
    static inline void interrupt();

  private:

    /*!
     * This type enumerates the different internal states. STATE_HIGH means that the
     * active channel is at high level. STATE_LOW means that the active channel is at
     * low level. STATE_SYNC means that no channel is active and we are waiting for
     * the frame to last 20ms (50Hz).
     */
    typedef enum
    {
      STATE_SYNC,
      STATE_HIGH,
      STATE_LOW
    } state_t;

    /*!
     * This type stores the high and low levels durations of a PWM cycle. A cycle
     * lasts exactly 2500us.
     */
    typedef struct
    {
      ServoTimer::duration_t high;
      ServoTimer::duration_t low;
    } cycle_t;

    /*! Internal state. */
    static state_t state;

    /*! Active channel. */
    static uint8_t channelId;

    /*! Buffered low level duration for the active channel. */
    static ServoTimer::duration_t lowDuration;

    /*!
     * Idle duration to synchronise the frame to 20ms (50Hz) after transmitting each
     * channel. Equal to 20ms - (2.5ms x number of channels used).
     */
    static const ServoTimer::duration_t syncDuration;

    /*! Array of PWM cycles, one per channel. */
    static cycle_t cycles[SERVO_CHANNELS];

    /*! Array of pins, one per channel. */
    static uint8_t pins[SERVO_CHANNELS];

    /*! Create a PWM cycle from a high level duration in microseconds. */
    static inline cycle_t createCycle(uint16_t time);

    /*! State machine action: set active channel to high level. */
    static inline void actionSetHigh();

    /*! State machine action: set active channel to low level. */
    static inline void actionSetLow();

    /*! State machine action: start frame synchronisation. */
    static inline void actionStartSync();
};

#endif // SERVO_H
