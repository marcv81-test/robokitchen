#include <Arduino.h>
#include "config.h"

#include "Magnetometer.h"

#include "Quaternion.h"

#ifdef IMU_MAGNET_CALIBRATED

Vector Magnetometer::mapSphere(const Vector& v)
{
    float xt = v.getX() - MAGNET_HARD_X;
    float yt = v.getY() - MAGNET_HARD_Y;
    float zt = v.getZ() - MAGNET_HARD_Z;
    return Vector(
      MAGNET_SOFT_XX * xt + MAGNET_SOFT_XY * yt + MAGNET_SOFT_XZ * zt,
      MAGNET_SOFT_YX * xt + MAGNET_SOFT_YY * yt + MAGNET_SOFT_YZ * zt,
      MAGNET_SOFT_ZX * xt + MAGNET_SOFT_ZY * yt + MAGNET_SOFT_ZZ * zt
    );
}

#endif
