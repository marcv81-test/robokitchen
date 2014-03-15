#include <Arduino.h>
#include "config.h"

#include "PID.h"

PIDController::PIDController(float kP, float kI, float kD) :
  kP(kP), kI(kI), kD(kD), control(0.0)
{
  error[0] = 0.0;
  errorDerivative[0] = 0.0;
}

void PIDController::update(float setpoint, float process, float time)
{
  // Update error history
  error[1] = error[0];
  error[0] = setpoint - process;

  // Calculate error change
  float errorChange = error[0] - error[1];

  // Update error derivative history
  errorDerivative[1] = errorDerivative[0];
  errorDerivative[0] = errorChange / time;

  // Update control
  control
    += kP * errorChange
    +  kI * error[0] * time
    +  kD * (errorDerivative[0] - errorDerivative[1]);
}

void PIDController::reset()
{
  control = 0.0;
  error[0] = 0.0;
  errorDerivative[0] = 0.0;
}
