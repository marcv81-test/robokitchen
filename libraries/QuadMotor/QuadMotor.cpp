#ifndef UNIT_TEST
  #include <Arduino.h>
#else
  #include "ArduinoMock.h"
#endif

#include "QuadMotor.h"

#include "ESC.h"

void QuadMotor::init()
{
  // Calibrate the max ESC PWM input
  for(int i=0 ; i<4 ; i++) ESC::setChannel(i, MOTOR_MAX);
  delay(3000);

  // Calibrate the min ESC PWM input (the motors stall below this value)
  for(int i=0 ; i<4 ; i++) ESC::setChannel(i, MOTOR_STALL);
  delay(2000);
}

void QuadMotor::stop()
{
  // Stop all the motors
  for(int i=0 ; i<4 ; i++) ESC::setChannel(i, MOTOR_OFF);
}

void QuadMotor::mix(float throttle, float roll, float pitch, float yaw)
{
  // Compute the motor mix
  int16_t motor[4];
  motor[MOTOR_FRONT] = MOTOR_IDLE + pitch - yaw;
  motor[MOTOR_LEFT] = MOTOR_IDLE + roll + yaw;
  motor[MOTOR_BACK] = MOTOR_IDLE + pitch - yaw;
  motor[MOTOR_RIGHT] = MOTOR_IDLE + roll + yaw;

  // Limit the motors
  for(int i=0 ; i<4 ; i++)
  {
    if(motor[i] < MOTOR_MIN) motor[i] = MOTOR_MIN;
    else if(motor[i] > MOTOR_MAX) motor[i] = MOTOR_MAX;
  }

  // Adjust the throttle contribution
  int16_t motorMin = motor[0], motorMax = motor[0];
  for(int i=1 ; i<4 ; i++)
  {
    if(motor[i] < motorMin) motorMin = motor[i];
    else if(motor[i] > motorMax) motorMax = motor[i];
  }
  if(motorMin + throttle < MOTOR_MIN) throttle = MOTOR_MIN - motorMin;
  else if(motorMax + throttle > MOTOR_MAX) throttle = MOTOR_MAX - motorMax;

  // Update the ESCs
  for(int i=0 ; i<4 ; i++) ESC::setChannel(i, motor[i] + throttle);
}
