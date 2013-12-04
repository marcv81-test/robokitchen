#ifndef IMU_H
#define IMU_H

#include "Quaternion.h"

/*!
 * This class provides the logic to combine accelerometer and gyroscope data into
 * an attitude quaternion. Call init() once, then refresh() (expensive) in the
 * main loop to refresh the attitude.
 */
class IMU
{
  public:

    static void init();
    static void refresh();

    // Calibration
    static void gyroCalibration();

    static uint32_t getLoopTime() { return loopTime; }
    static const Quaternion& getAttitude() { return attitude; }

  private:

    // Loop timer
    static uint32_t loopTime, loopStartTime, loopStopTime;

    // Attitude
    static Quaternion attitude;

    // Calibration
    static AxisAngle gyroBias;
    #ifdef IMU_ACCEL_ENABLE
      static Vector accelReference;
    #endif
};

#endif // IMU_H
