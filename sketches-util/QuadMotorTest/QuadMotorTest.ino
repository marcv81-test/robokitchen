#include "config.h"

#include "ESC.h"
#include "PPMRX.h"

int8_t throttle, rudder, elevator, ailerons;

#define ABS(X) ((X) < 0) ? -(X) : (X)

void initMotors()
{
  for(int i=0 ; i<4 ; i++) ESC::setChannel(i, MOTOR_MAX);
  delay(3000);

  for(int i=0 ; i<4 ; i++) ESC::setChannel(i, MOTOR_STALL);
  delay(2000);
}

void stopMotors()
{
  for(int i=0 ; i<4 ; i++) ESC::setChannel(i, MOTOR_OFF);
}

void updateMotors()
{
  ESC::setChannel(MOTOR_FRONT, MOTOR_OFF + 10 * ((uint16_t) ABS(throttle)));
  ESC::setChannel(MOTOR_LEFT, MOTOR_OFF + 10 * ((uint16_t) ABS(rudder)));
  ESC::setChannel(MOTOR_BACK, MOTOR_OFF + 10 * ((uint16_t) ABS(elevator)));
  ESC::setChannel(MOTOR_RIGHT, MOTOR_OFF + 10 * ((uint16_t) ABS(ailerons)));
}

void setup()
{
  ESC::init();
  initMotors();

  Serial.begin(115200);
  Serial.println("");
  Serial.println("---");

  PPMRX::init();
}

void loop()
{
  if(PPMRX::refresh())
  {
    throttle = PPMRX::getChannel(PPMRX_CHANNEL_THROTTLE);
    rudder = PPMRX::getChannel(PPMRX_CHANNEL_RUDDER);
    elevator = PPMRX::getChannel(PPMRX_CHANNEL_ELEVATOR);
    ailerons = PPMRX::getChannel(PPMRX_CHANNEL_AILERONS);

    updateMotors();
  }
  else
  {
    stopMotors();
  }
}
