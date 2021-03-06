#include <Arduino.h>
#include "config.h"

#include "Servo.h"

ISR(TIMER2_OVF_vect) { ServoTimer::interrupt(); }

// ================================ ServoTimer ================================ //

void ServoTimer::reset(ServoTimer::duration_t duration)
{
  ServoTimer::duration = duration;
  counter = 0;
  TCNT2 = 0;
}

ServoTimer::duration_t ServoTimer::createDuration(uint16_t time)
{
  duration_t duration;
  duration.primary = time >> 7;
  duration.secondary = 255 - ((time - ((time >> 7) << 7)) << 1);
  return duration;
}

void ServoTimer::init()
{
  TIMSK2 = 0;
  TCCR2A = 0;
  TCCR2B = _BV(CS21);
  TCNT2 = 0;
  TIFR2 = _BV(TOV2);
  TIMSK2 = _BV(TOIE2);
}

void ServoTimer::interrupt()
{
  counter++;
  if(counter == duration.primary) TCNT2 = duration.secondary;
  else if(counter > duration.primary) Servo::interrupt();
}

uint8_t ServoTimer::counter;
ServoTimer::duration_t ServoTimer::duration;

// ================================ Servo ================================ //

void Servo::setChannel(uint8_t channelId, uint16_t time)
{
  if(channelId < SERVO_CHANNELS)
  {
    cycle_t cycle = createCycle(time);
    cli();
    Servo::cycles[channelId] = cycle;
    sei();
  }
}

void Servo::init()
{
  for(uint8_t i=0 ; i<SERVO_CHANNELS ; i++)
  {
    pinMode(pins[i], OUTPUT);
    setChannel(i, (SERVO_HIGH_LEVEL_MIN_TIME + SERVO_HIGH_LEVEL_MAX_TIME) / 2);
  }
  actionStartSync();
  ServoTimer::init();
}

void Servo::interrupt()
{
  switch(state)
  {
    case STATE_HIGH:
      actionSetLow();
    break;

    case STATE_LOW:
      channelId++;
      if(channelId < SERVO_CHANNELS)
        actionSetHigh();
      else
        actionStartSync();
    break;

    case STATE_SYNC: default:
      channelId = 0;
      actionSetHigh();
    break;
  }
}

uint8_t Servo::channelId;
Servo::state_t Servo::state;

ServoTimer::duration_t Servo::lowDuration;

uint8_t Servo::pins[SERVO_CHANNELS] = SERVO_PINS;
Servo::cycle_t Servo::cycles[SERVO_CHANNELS];

const ServoTimer::duration_t Servo::syncDuration =
  ServoTimer::createDuration(SERVO_FRAME_SYNC_TIME);

Servo::cycle_t Servo::createCycle(uint16_t time)
{
  if(time < SERVO_HIGH_LEVEL_MIN_TIME) time = SERVO_HIGH_LEVEL_MIN_TIME;
  else if(time > SERVO_HIGH_LEVEL_MAX_TIME) time = SERVO_HIGH_LEVEL_MAX_TIME;

  cycle_t cycle;
  cycle.high = ServoTimer::createDuration(time - SERVO_DELAY);
  cycle.low = ServoTimer::createDuration(SERVO_CYCLE_TOTAL_TIME - time - SERVO_DELAY);
  return cycle;
}

void Servo::actionSetHigh()
{
  digitalWrite(pins[channelId], HIGH);
  ServoTimer::reset(cycles[channelId].high);
  lowDuration = cycles[channelId].low;
  state = STATE_HIGH;
}

void Servo::actionSetLow()
{
  digitalWrite(pins[channelId], LOW);
  ServoTimer::reset(lowDuration);
  state = STATE_LOW;
}

void Servo::actionStartSync()
{
  ServoTimer::reset(syncDuration);
  state = STATE_SYNC;
}
