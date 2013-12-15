#ifndef PID_H
#define PID_H

#include <Arduino.h>
#include "config.h"

/*!
 * This class provides a PID controller implementation. Controllers are used to drive
 * processes: they provide a control value (process input) so that the process value
 * (process output) reaches a defined setpoint. PID controllers work in closed loop:
 * the process value is fed back to the controller to calculate the next control value.
 *
 * This implementation uses back-calculation to prevent integral wind up. The control
 * value is initialised to 0 and stays between -controlMax and +controlMax.
 */
class PIDController
{
  protected:
    float kP, kI, kD, controlMax;
    float control, totalError, previousProcess;

  public:
    PIDController(float kP, float kI, float kD, float controlMax);
    float getControl() const { return control; }
    void update(float setpoint, float process);
};

#endif // PID_H
