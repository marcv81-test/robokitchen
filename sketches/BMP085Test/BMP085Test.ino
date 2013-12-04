#include "config.h"

#include "I2C.h"
#include "BMP085.h"

void setup()
{
  I2C::begin();

  #ifdef BMP085_DEBUG
    Serial.begin(115200);
    Serial.println("");
    Serial.println("---");
  #endif

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
