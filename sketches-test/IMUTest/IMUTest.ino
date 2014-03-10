#include "config.h"

#include "I2C.h"
#include "IMU.h"

// Counter to reduce FPS
uint8_t counter = 0;

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
  if(IMU::refresh())
  {
    // Reduce display frame rate
    if(counter++ == CONFIG_FRAME_DROP)
    {
      counter = 0;

      #ifdef CONFIG_VECTOR_OUTPUT
        Vector forward = Vector(1.0, 0.0, 0.0).rotate(IMU::getAttitude());
        Vector down = Vector(0.0, 0.0, 1.0).rotate(IMU::getAttitude());
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

      Serial.print(IMU::getLoopTime());
      Serial.println("");
    }
  }
}
