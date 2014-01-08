#include "config.h"

#include "I2C.h"
#include "IMU.h"
#include "Servo.h"
#include "PID.h"

// PID controllers
PIDController rollPID = PIDController(PID_KP, PID_KI, PID_KD, PID_CONTROL_MAX);
PIDController pitchPID = PIDController(PID_KP, PID_KI, PID_KD, PID_CONTROL_MAX);

void setup()
{
  Serial.begin(115200);
  Serial.println("");
  Serial.println("---");

  I2C::begin();
  IMU::init();
  Servo::init();
}

void loop()
{
  if(IMU::refresh())
  {
    // The IMU points backward so pitch and roll need a sign swap
    Quaternion attitude = IMU::getAttitude();
    float roll = -attitude.getRoll();
    float pitch = -attitude.getPitch();

    // Update controllers
    rollPID.update(0.0, roll);
    pitchPID.update(0.0, pitch);

    // Set servo output
    Servo::setChannel(0, 1500 + pitchPID.getControl());
    Servo::setChannel(1, 1500 + rollPID.getControl());
  }
}
