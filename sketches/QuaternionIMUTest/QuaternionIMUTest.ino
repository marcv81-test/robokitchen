#include "config.h"
#include <math.h>

#include "I2C.h"
#include "Quaternion.h"
#include "QuaternionIMU.h"

// Counter to reduce FPS
uint8_t counter = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println("");
  Serial.println("---");

  I2C::begin();
  QuaternionIMU::init();
}

void loop()
{
  QuaternionIMU::refresh();

  // Display
  if(counter++ == SKETCH_FRAME_DROP)
  {
    counter = 0;

    #ifdef SKETCH_VECTOR_OUTPUT
      Vector forward = Vector(1.0, 0.0, 0.0).rotate(QuaternionIMU::getAttitude());
      Vector down = Vector(0.0, 0.0, 1.0).rotate(QuaternionIMU::getAttitude());
      Serial.print(forward.getX());
      Serial.print(",");
      Serial.print(forward.getY());
      Serial.print(",");
      Serial.print(forward.getZ());
      Serial.print(",");
      Serial.print(down.getX());
      Serial.print(",");
      Serial.print(down.getY());
      Serial.print(",");
      Serial.print(down.getZ());
      Serial.print(",");
    #endif

    Serial.print(QuaternionIMU::getLoopTime());
    Serial.println("");
  }
}
