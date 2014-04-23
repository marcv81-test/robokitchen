#include <Arduino.h>
#include "config.h"

#include "IMU.h"

#include "MPU6050.h"
#ifdef IMU_MAGNET_ENABLE
  #include "HMC5883L.h"
#endif

#ifdef IMU_DEBUG
  #define DEBUG_SKIP 50
#endif

// ================================ Public ================================ //

void IMU::init()
{
  // Initialise sensors
  MPU6050::init();
  #ifdef IMU_MAGNET_ENABLE
    HMC5883L::init();
  #endif

  // Calibrate gyroscope
  #ifdef IMU_GYRO_CALIBRATION
    calibrateGyro();
  #endif

  loopStartTime = micros();
}

uint8_t IMU::refresh()
{
  // Calculate loop time
  loopStopTime = micros();
  loopTime = (loopStopTime - loopStartTime) / 1000000.0;
  loopStartTime = loopStopTime;

  // Refresh gyroscope and accelerometer
  if(!MPU6050::refresh()) return 0;

  // Calculate gyroscope rotation
  gyro = AxisAngle(IMU_GYRO_X, IMU_GYRO_Y, IMU_GYRO_Z);
  #ifdef IMU_GYRO_CALIBRATION
    gyro -= gyroBias;
  #endif
  gyro *= loopTime;

  // Correct accelerometer error
  #ifdef IMU_ACCEL_ENABLE
    Vector up = Vector(0.0, 0.0, -1.0).rotate(attitude.conjugate());
    accel = Vector(IMU_ACCEL_X, IMU_ACCEL_Y, IMU_ACCEL_Z);
    AxisAngle accelError = AxisAngle::fromTwoVectors(up, accel);
    gyro -= (accelError * IMU_ACCEL_COEFF);
  #endif

  // Correct magnetometer error
  #ifdef IMU_MAGNET_ENABLE
    if(HMC5883L::refresh())
    {
      #ifdef IMU_MAGNET_CALIBRATION
        float xTemp = IMU_MAGNET_X - MAGNET_HARD_X;
        float yTemp = IMU_MAGNET_Y - MAGNET_HARD_Y;
        float zTemp = IMU_MAGNET_Z - MAGNET_HARD_Z;
        magnet = Vector(
          MAGNET_SOFT_XX * xTemp + MAGNET_SOFT_XY * yTemp + MAGNET_SOFT_XZ * zTemp,
          MAGNET_SOFT_YX * xTemp + MAGNET_SOFT_YY * yTemp + MAGNET_SOFT_YZ * zTemp,
          MAGNET_SOFT_ZX * xTemp + MAGNET_SOFT_ZY * yTemp + MAGNET_SOFT_ZZ * zTemp
        );
      #else
        magnet = Vector(IMU_MAGNET_X, IMU_MAGNET_Y, IMU_MAGNET_Z);
      #endif
      magnet = magnet.reject(up);
      Vector north = Vector(1.0, 0.0, 0.0).rotate(attitude.conjugate());
      AxisAngle magnetError = AxisAngle::fromTwoVectors(north, magnet);
      gyro -= (magnetError * IMU_MAGNET_COEFF);
    }
  #endif

  // Update attitude
  attitude = attitude * Quaternion::fromAxisAngle(gyro);
  attitude.normalise();

  return 1;
}

#ifdef IMU_DEBUG
  void IMU::debug()
  {
    if(++debugCounter == DEBUG_SKIP)
    {
      debugCounter = 0;

      Vector forward = Vector(1.0, 0.0, 0.0).rotate(IMU::getAttitude());
      Serial.print("Forward:");
      Serial.print(forward.getX());
      Serial.print(",");
      Serial.print(forward.getY());
      Serial.print(",");
      Serial.print(forward.getZ());
      Serial.println();

      Vector down = Vector(0.0, 0.0, 1.0).rotate(IMU::getAttitude());
      Serial.print("Down:");
      Serial.print(down.getX());
      Serial.print(",");
      Serial.print(down.getY());
      Serial.print(",");
      Serial.print(down.getZ());
      Serial.println();

      #ifdef IMU_ACCEL_ENABLE
        Vector accel = IMU::getAccel();
        Serial.print("Accel:");
        Serial.print(accel.getX(), 4);
        Serial.print(",");
        Serial.print(accel.getY(), 4);
        Serial.print(",");
        Serial.print(accel.getZ(), 4);
        Serial.println();
      #endif

      #ifdef IMU_MAGNET_ENABLE
        Vector magnet = IMU::getMagnet();
        Serial.print("Magnet:");
        Serial.print(magnet.getX(), 4);
        Serial.print(",");
        Serial.print(magnet.getY(), 4);
        Serial.print(",");
        Serial.print(magnet.getZ(), 4);
        Serial.println();
      #endif
    }
  }
#endif

// ================================ Private ================================ //

float IMU::loopTime = 0.0;
uint32_t IMU::loopStartTime = 0;
uint32_t IMU::loopStopTime = 0;

Quaternion IMU::attitude = Quaternion();

AxisAngle IMU::gyro = AxisAngle();
#ifdef IMU_ACCEL_ENABLE
  Vector IMU::accel = Vector();
#endif
#ifdef IMU_MAGNET_ENABLE
  Vector IMU::magnet = Vector();
#endif

#ifdef IMU_GYRO_CALIBRATION
  AxisAngle IMU::gyroBias = AxisAngle();
#endif

#ifdef IMU_GYRO_CALIBRATION
  void IMU::calibrateGyro()
  {
    gyroBias = AxisAngle();
    for(uint16_t i=0; i<IMU_GYRO_CALIBRATION_LENGTH; i++)
    {
      MPU6050::refresh();
      gyroBias += AxisAngle(IMU_GYRO_X, IMU_GYRO_Y, IMU_GYRO_Z);
    }
    gyroBias /= IMU_GYRO_CALIBRATION_LENGTH;
  }
#endif

#ifdef IMU_DEBUG
  uint8_t IMU::debugCounter = 0;
#endif
