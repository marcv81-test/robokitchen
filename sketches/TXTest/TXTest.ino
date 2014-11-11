#include <Arduino.h>
#include "config.h"

#include "PPMTX.h"

void setup()
{
  Serial.begin(115200);
  Serial.println("");
  Serial.println("---");

  PPMTX::init();

  PPMTX::setChannel(0, 0);
  PPMTX::setChannel(1, 0);
  PPMTX::setChannel(2, 0);
  PPMTX::setChannel(3, 0);
  PPMTX::setChannel(4, 0);
  PPMTX::setChannel(5, 0);
  PPMTX::setChannel(6, 0);
}

void loop()
{
  PPMTX::sendFrame();
}
