#include <Arduino.h>
#include "config.h"

#include "PID.h"

#define BACK_CALCULATION if (kI != 0) totalError = (control - termP - termD) / kI

PIDController::PIDController(float kP, float kI, float kD, float controlMax) :
  kP(kP), kI(kI), kD(kD), controlMax(controlMax),
  control(0.0), totalError(0.0), previousProcess(0.0) { }

void PIDController::update(float setpoint, float process)
{
  // proportional term
  float error = setpoint - process;
  float termP = kP * error;

  // integral term
  totalError += error;
  float termI = kI * totalError;

  // derivative term
  float termD = kD * (process - previousProcess);
  previousProcess = process;

  // calculate the control
  control = termP + termI + termD;

  // limit control
  if(control > controlMax)
  {
    control = controlMax;
    BACK_CALCULATION;
  }
  else if(control < -controlMax)
  {
    control = -controlMax;
    BACK_CALCULATION;
  }
}
