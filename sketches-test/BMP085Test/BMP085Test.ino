#include "config.h"

#include "I2C.h"
#include "BMP085.h"

void setup()
{
  Serial.begin(115200);
  Serial.println("");
  Serial.println("---");

  I2C::begin();
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
