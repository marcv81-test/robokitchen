#ifndef IMU_H
#define IMU_H

#include <Arduino.h>
#include "config.h"

// Validate dependencies
#if defined IMU_MAGNET_ENABLE && !defined IMU_ACCEL_ENABLE
  #error IMU_MAGNET_ENABLE depends on IMU_ACCEL_ENABLE
#endif
#if defined IMU_ACCEL_ENABLE && !defined IMU_ACCEL_COEFF
  #error IMU_ACCEL_COEFF is required with IMU_ACCEL_ENABLE
#endif
#if defined IMU_MAGNET_ENABLE && !defined IMU_MAGNET_COEFF
  #error IMU_MAGNET_COEFF is required with IMU_MAGNET_ENABLE
#endif

// Default number of samples for gyroscope calibration
#if defined IMU_GYRO_CALIBRATION && !defined IMU_GYRO_CALIBRATION_LENGTH
  #define IMU_GYRO_CALIBRATION_LENGTH 250
#endif

// Validate magnetometer calibration data
#if defined IMU_MAGNET_ENABLE && defined IMU_MAGNET_CALIBRATION && ( \
  !defined MAGNET_HARD_X  || !defined MAGNET_HARD_Y  || !defined MAGNET_HARD_Z  || \
  !defined MAGNET_SOFT_XX || !defined MAGNET_SOFT_XY || !defined MAGNET_SOFT_XZ || \
  !defined MAGNET_SOFT_YX || !defined MAGNET_SOFT_YY || !defined MAGNET_SOFT_YZ || \
  !defined MAGNET_SOFT_ZX || !defined MAGNET_SOFT_ZY || !defined MAGNET_SOFT_ZZ )
  #error Incomplete magnetometer calibration data
#endif

#include "Quaternion.h"

/*!
 * This class provides the logic to combine gyroscope, accelerometer and magnetometer
 * data into an attitude quaternion. Call init() once, then refresh() (expensive) in
 * the main loop to refresh the attitude.
 */
class IMU
{
  public:

    static void init();
    static uint8_t refresh();

    static float getLoopTime() { return loopTime; }
    static const Quaternion& getAttitude() { return attitude; }

    // Sensors vectors accessors
    static const AxisAngle getGyro() { return gyro; }
    #ifdef IMU_ACCEL_ENABLE
      static const Vector getAccel() { return accel; }
    #endif
    #ifdef IMU_MAGNET_ENABLE
      static const Vector getMagnet() { return magnet; }
    #endif

    #ifdef IMU_ANGLES
      static float getRoll() { return roll; }
      static float getPitch() { return pitch; }
      static float getYaw() { return yaw; }
      static float getRollRate() { return rollRate; }
      static float getPitchRate() { return pitchRate; }
      static float getYawRate() { return yawRate; }
    #endif

    // Output debug data
    #ifdef IMU_DEBUG
      static void debug();
    #endif

  private:

    // Loop timer
    static float loopTime;
    static uint32_t loopStartTime, loopStopTime;

    // Attitude
    static Quaternion attitude;

    // Sensors vectors
    static AxisAngle gyro;
    #ifdef IMU_ACCEL_ENABLE
      static Vector accel;
    #endif
    #ifdef IMU_MAGNET_ENABLE
      static Vector magnet;
    #endif

    // Angles
    #ifdef IMU_ANGLES
      static float roll, pitch, yaw;
      static float rollRate, pitchRate, yawRate;
    #endif

    // Gyroscope calibration data
    #ifdef IMU_GYRO_CALIBRATION
      static AxisAngle gyroBias;
    #endif

    // Gyroscope calibration procedure
    #ifdef IMU_GYRO_CALIBRATION
      static void calibrateGyro();
    #endif

    #ifdef IMU_DEBUG
      static uint8_t debugCounter;
    #endif
};

#endif // IMU_H
