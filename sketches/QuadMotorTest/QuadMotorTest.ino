#include "config.h"

#include "ESC.h"
#include "PPMRX.h"
#include "QuadMotor.h"

int8_t throttle, rudder, elevator, ailerons, aux;

#define ABS(X) ((X) < 0) ? -(X) : (X)

void updateMotors()
{
  // Prepare output
  int16_t output = -10 * aux + MOTOR_IDLE;
  if(output < MOTOR_OFF) { output = MOTOR_OFF; }
  else if(output > MOTOR_MAX) { output = MOTOR_MAX; }

  // Front and back motors
  if(abs(throttle) > 50)
  {
    ESC::setChannel(MOTOR_FRONT, output);
    ESC::setChannel(MOTOR_BACK, output);
  }
  else
  {
    ESC::setChannel(MOTOR_FRONT, elevator < -50 ? output : MOTOR_OFF);
    ESC::setChannel(MOTOR_BACK, elevator > 50 ? output : MOTOR_OFF);
  }

  // Left and right motors
  if(abs(rudder) > 50)
  {
    ESC::setChannel(MOTOR_LEFT, output);
    ESC::setChannel(MOTOR_RIGHT, output);
  }
  else
  {
    ESC::setChannel(MOTOR_LEFT, ailerons < -50 ? output : MOTOR_OFF);
    ESC::setChannel(MOTOR_RIGHT, ailerons > 50 ? output : MOTOR_OFF);
  }

  Serial.print("Output:");
  Serial.print(output);
  Serial.println("");
}

void setup()
{
  ESC::init();
  QuadMotor::init();

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
    aux = PPMRX::getChannel(PPMRX_CHANNEL_AUX);

    updateMotors();
  }
  else
  {
    QuadMotor::stop();
  }
}
