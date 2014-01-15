#include <Arduino.h>
#include "config.h"

#include "IMU.h"

#include "MPU6050.h"
#ifdef IMU_MAGNET_ENABLE
  #include "HMC5883L.h"
  #include "Magnetometer.h"
#endif

// ================================ Public ================================ //

void IMU::init()
{
  // Initialise sensors
  MPU6050::init();
  #ifdef IMU_MAGNET_ENABLE
    HMC5883L::init();
  #endif

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

  #if defined (IMU_ACCEL_ENABLE) || (defined IMU_MAGNET_ENABLE)
    Vector up = Vector(0.0, 0.0, -1.0).rotate(attitude.conjugate());
  #endif

  // Correct accelerometer error
  #ifdef IMU_ACCEL_ENABLE
    Vector accel = Vector(IMU_ACCEL_X, IMU_ACCEL_Y, IMU_ACCEL_Z);
    AxisAngle accelError = AxisAngle::fromTwoVectors(up, accel);
    gyro -= (accelError * IMU_ACCEL_COEFF);
  #endif

  // Correct magnetometer error
  #ifdef IMU_MAGNET_ENABLE
    if(HMC5883L::refresh())
    {
      Vector north = Vector(1.0, 0.0, 0.0).rotate(attitude.conjugate());
      Vector magnet = Vector(IMU_MAGNET_X, IMU_MAGNET_Y, IMU_MAGNET_Z);
      #ifdef IMU_MAGNET_CALIBRATED
        magnet = Magnetometer::mapSphere(magnet);
      #endif
      magnet = magnet.reject(up);
      AxisAngle magnetError = AxisAngle::fromTwoVectors(north, magnet);
      gyro -= (magnetError * IMU_MAGNET_COEFF);
    }
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
