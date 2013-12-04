#include "config.h"

#include "I2C.h"
#include "Endian.h"
#include "MPU6050.h"
#include "HMC5883L.h"

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
  Serial.print(IMU_ACCEL_X, 2);
  Serial.print(",");
  Serial.print(IMU_ACCEL_Y, 2);
  Serial.print(",");
  Serial.print(IMU_ACCEL_Z, 2);
  Serial.print(",");

  // Print magnetometer data
  Serial.print(IMU_MAGNET_X, 2);
  Serial.print(",");
  Serial.print(IMU_MAGNET_Y, 2);
  Serial.print(",");
  Serial.print(IMU_MAGNET_Z, 2);
  Serial.print(",");

  // Print gyroscope data
  Serial.print(IMU_GYRO_X, 2);
  Serial.print(",");
  Serial.print(IMU_GYRO_Y, 2);
  Serial.print(",");
  Serial.print(IMU_GYRO_Z, 2);
  Serial.println("");

  delay(100);
}
