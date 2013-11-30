#include <Arduino.h>
#include "config.h"

#include "QuaternionIMU.h"

#include "MPU6050.h"
#include "IMU.h"

// ================================ Public ================================ //

void QuaternionIMU::init()
{
  // Initialise sensors
  MPU6050::init();

  // Calibrate sensors
  gyroCalibration();
}

void QuaternionIMU::refresh()
{
  // Calculate loop time
  loopStopTime = micros();
  loopTime = loopStopTime - loopStartTime;
  loopStartTime = loopStopTime;

  // Refresh sensors
  MPU6050::refresh();

  // Calculate gyroscope rotation
  AxisAngle gyro = AxisAngle(GYRO_X, GYRO_Y, GYRO_Z) - gyroBias;
  gyro *= (loopTime / 1000000.0);

  // Correct accelerometer error
  #ifdef IMU_ACCEL_ENABLE
    Vector accel = Vector(ACCEL_X, ACCEL_Y, ACCEL_Z);
    Vector accelReferenceRot = accelReference.rotate(attitude.conjugate());
    AxisAngle accelError = AxisAngle::fromTwoVectors(accelReferenceRot, accel);
    gyro -= (accelError * IMU_ACCEL_COEFF);
  #endif

  // Update attitude
  attitude = attitude * Quaternion::fromAxisAngle(gyro);
  attitude.normalise();
}

void QuaternionIMU::gyroCalibration()
{
  gyroBias = AxisAngle();
  for(uint16_t i=0; i<IMU_GYRO_CALIB_SAMPLE_SIZE; i++)
  {
    MPU6050::refresh();
    gyroBias += AxisAngle(GYRO_X, GYRO_Y, GYRO_Z);
  }
  gyroBias /= IMU_GYRO_CALIB_SAMPLE_SIZE;
}

// ================================ Private ================================ //

Quaternion QuaternionIMU::attitude = Quaternion();

uint32_t QuaternionIMU::loopTime = 0;
uint32_t QuaternionIMU::loopStartTime = 0;
uint32_t QuaternionIMU::loopStopTime = 0;

AxisAngle QuaternionIMU::gyroBias = AxisAngle();
#ifdef IMU_ACCEL_ENABLE
  Vector QuaternionIMU::accelReference = Vector(0.0, 0.0, -1.0);
#endif
