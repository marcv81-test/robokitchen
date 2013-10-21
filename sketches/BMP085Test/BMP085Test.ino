#include "I2C.h"
#include "BMP085.h"
#include "Endian.h"

void setup()
{
  I2C::begin();

  Serial.begin(115200);
  Serial.println("");
  Serial.println("---");

  BMP085::init();
}

void loop()
{
  if(BMP085::refresh())
  {
    Serial.print(BMP085::getPressure());
    Serial.println();
  }
}
