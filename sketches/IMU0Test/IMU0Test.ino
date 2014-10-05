#include "config.h"

#include "I2C.h"
#include "IMU.h"

void setup()
{
  Serial.begin(115200);
  Serial.println("");
  Serial.println("---");

  I2C::begin();
  IMU::init();
}

void loop()
{
  IMU::refresh();

  #ifdef IMU_DEBUG
    IMU::debug();
  #endif
}
