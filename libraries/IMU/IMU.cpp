#include <Arduino.h>
#include "config.h"

#include "IMU.h"

#include "MPU6050.h"

// ================================ Public ================================ //

void IMU::init()
{
  // Initialise sensors
  MPU6050::init();

  // Calibrate sensors
  gyroCalibration();
}

uint8_t IMU::refresh()
{
  // Calculate loop time
  loopStopTime = micros();
  loopTime = loopStopTime - loopStartTime;
  loopStartTime = loopStopTime;

  // Refresh sensors
  if(!MPU6050::refresh()) return 0;

  // Calculate gyroscope rotation
  AxisAngle gyro = AxisAngle(IMU_GYRO_X, IMU_GYRO_Y, IMU_GYRO_Z) - gyroBias;
  gyro *= (loopTime / 1000000.0);

  // Correct accelerometer error
  #ifdef IMU_ACCEL_ENABLE
    Vector accel = Vector(IMU_ACCEL_X, IMU_ACCEL_Y, IMU_ACCEL_Z);
    Vector accelReferenceRot = accelReference.rotate(attitude.conjugate());
    AxisAngle accelError = AxisAngle::fromTwoVectors(accelReferenceRot, accel);
    gyro -= (accelError * IMU_ACCEL_COEFF);
  #endif

  // Update attitude
  attitude = attitude * Quaternion::fromAxisAngle(gyro);
  attitude.normalise();

  return 1;
}

void IMU::gyroCalibration()
{
  gyroBias = AxisAngle();
  for(uint16_t i=0; i<IMU_GYRO_CALIB_SAMPLE_SIZE; i++)
  {
    MPU6050::refresh();
    gyroBias += AxisAngle(IMU_GYRO_X, IMU_GYRO_Y, IMU_GYRO_Z);
  }
  gyroBias /= IMU_GYRO_CALIB_SAMPLE_SIZE;
}

// ================================ Private ================================ //

Quaternion IMU::attitude = Quaternion();

uint32_t IMU::loopTime = 0;
uint32_t IMU::loopStartTime = 0;
uint32_t IMU::loopStopTime = 0;

AxisAngle IMU::gyroBias = AxisAngle();
#ifdef IMU_ACCEL_ENABLE
  Vector IMU::accelReference = Vector(0.0, 0.0, -1.0);
#endif
