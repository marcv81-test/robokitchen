#ifndef PID_H
#define PID_H

#include <Arduino.h>
#include "config.h"

/*!
 * This class provides a PID controller using the velocity/incremental implementation.
 * Controllers are used to drive processes: they provide a control value (process input)
 * so that the process value (process output) reaches a defined setpoint. PID controllers
 * work in closed loop: the process value is fed back to the controller to calculate the
 * next control value.
 */
class PIDController
{
  protected:
    float kP, kI, kD;
    float control;
    float error[2], errorDerivative[2];

  public:
    PIDController(float kP, float kI, float kD);
    float getControl() const { return control; }
    void update(float setpoint, float process, float time);
    void reset();
};

#endif // PID_H
