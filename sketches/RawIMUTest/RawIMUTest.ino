#include "config.h"

#include "I2C.h"
#include "Endian.h"
#include "MPU6050.h"
#include "HMC5883L.h"
#include "IMU.h"

void setup()
{
  I2C::begin();

  Serial.begin(115200);
  Serial.println("");
  Serial.println("---");

  MPU6050::init();
  HMC5883L::init();
}

void loop()
{
  HMC5883L::refresh();
  MPU6050::refresh();

  // Print accelerometer data
  Serial.print(ACCEL_X, 2);
  Serial.print(",");
  Serial.print(ACCEL_Y, 2);
  Serial.print(",");
  Serial.print(ACCEL_Z, 2);
  Serial.print(",");

  // Print magnetometer data
  Serial.print(MAGNET_X, 2);
  Serial.print(",");
  Serial.print(MAGNET_Y, 2);
  Serial.print(",");
  Serial.print(MAGNET_Z, 2);
  Serial.print(",");

  // Print gyroscope data
  Serial.print(GYRO_X, 2);
  Serial.print(",");
  Serial.print(GYRO_Y, 2);
  Serial.print(",");
  Serial.print(GYRO_Z, 2);
  Serial.println("");

  delay(100);
}
