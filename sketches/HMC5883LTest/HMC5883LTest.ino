#include "config.h"

#include "I2C.h"
#include "HMC5883L.h"

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
    // print magnetometer data
    Serial.print(HMC5883L::getMagnetX(), 4);
    Serial.print(",");
    Serial.print(HMC5883L::getMagnetY(), 4);
    Serial.print(",");
    Serial.print(HMC5883L::getMagnetZ(), 4);
    Serial.println("");
  }
}
