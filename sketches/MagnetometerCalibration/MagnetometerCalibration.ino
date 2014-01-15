#include "config.h"

#include "I2C.h"
#include "HMC5883L.h"
#include "Quaternion.h"
#include "Magnetometer.h"

void setup()
{
  Serial.begin(115200);
  Serial.println("");
  Serial.println("---");

  I2C::begin();
  HMC5883L::init();
}

void loop()
{
  if(HMC5883L::refresh())
  {
    // read magnetometer vector
    Vector magnet = Vector(
      IMU_MAGNET_X,
      IMU_MAGNET_Y,
      IMU_MAGNET_Z
    );

    // map magnetometer vector to unit sphere
    #ifdef IMU_MAGNET_CALIBRATED
      magnet = Magnetometer::mapSphere(magnet);
    #endif

    // print magnetometer vector
    Serial.print(magnet.getX(), 4);
    Serial.print(",");
    Serial.print(magnet.getY(), 4);
    Serial.print(",");
    Serial.print(magnet.getZ(), 4);
    Serial.println("");
  }
}
