#ifndef MAGNETOMETER_H
#define MAGNETOMETER_H

#include <Arduino.h>
#include "config.h"

#include "Quaternion.h"

#ifdef IMU_MAGNET_CALIBRATED
  #if \
    !defined MAGNET_HARD_X  || !defined MAGNET_HARD_Y  || !defined MAGNET_HARD_Z  || \
    !defined MAGNET_SOFT_XX || !defined MAGNET_SOFT_XY || !defined MAGNET_SOFT_XZ || \
    !defined MAGNET_SOFT_YX || !defined MAGNET_SOFT_YY || !defined MAGNET_SOFT_YZ || \
    !defined MAGNET_SOFT_ZX || !defined MAGNET_SOFT_ZY || !defined MAGNET_SOFT_ZZ
    #error Missing magnetometer calibration
  #endif
#endif

 /*!
 * This class provides routines to correct the magnetometer vector.
 */
class Magnetometer
{
  public:

    /*!
     * This function maps the magnetometer vector to an origin-centered unit sphere.
     */
    #ifdef IMU_MAGNET_CALIBRATED
      static Vector mapSphere(const Vector& v);
    #endif
};

#endif // MAGNETOMETER_H
