#include "I2C.h"
#include "HMC5883L.h"
#include "Endian.h"

void setup()
{
  I2C::begin();

  Serial.begin(115200);
  Serial.println("");
  Serial.println("---");

  HMC5883L::init();
}

void loop()
{
  HMC5883L::refresh();

  // print magnetometer data
  Serial.print(HMC5883L::getMagnetX(), 4);
  Serial.print(F(","));
  Serial.print(HMC5883L::getMagnetY(), 4);
  Serial.print(F(","));
  Serial.print(HMC5883L::getMagnetZ(), 4);
  Serial.println(F(""));

}
