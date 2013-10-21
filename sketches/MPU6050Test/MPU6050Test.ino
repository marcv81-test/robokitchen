#include "I2C.h"
#include "MPU6050.h"
#include "Endian.h"

void setup()
{
  I2C::begin();

  Serial.begin(115200);
  Serial.println("");
  Serial.println("---");

  MPU6050::init();
}

void loop()
{
  MPU6050::refresh();

  // print accelerometer data
  Serial.print(MPU6050::getAccelX(), 4);
  Serial.print(F(","));
  Serial.print(MPU6050::getAccelY(), 4);
  Serial.print(F(","));
  Serial.print(MPU6050::getAccelZ(), 4);
  Serial.print(F(","));

  // print gyroscope data
  Serial.print(MPU6050::getGyroX(), 4);
  Serial.print(F(","));
  Serial.print(MPU6050::getGyroY(), 4);
  Serial.print(F(","));
  Serial.print(MPU6050::getGyroZ(), 4);
  Serial.println(F(""));

}
