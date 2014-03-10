#include "config.h"

#include "I2C.h"
#include "MPU6050.h"

void setup()
{
  Serial.begin(115200);
  Serial.println("");
  Serial.println("---");

  I2C::begin();
  MPU6050::init();
}

void loop()
{
  if(MPU6050::refresh())
  {
    // print accelerometer data
    Serial.print(MPU6050::getAccelX(), 4);
    Serial.print(",");
    Serial.print(MPU6050::getAccelY(), 4);
    Serial.print(",");
    Serial.print(MPU6050::getAccelZ(), 4);
    Serial.print(",");

    // print gyroscope data
    Serial.print(MPU6050::getGyroX(), 4);
    Serial.print(",");
    Serial.print(MPU6050::getGyroY(), 4);
    Serial.print(",");
    Serial.print(MPU6050::getGyroZ(), 4);
    Serial.println("");
  }
}
