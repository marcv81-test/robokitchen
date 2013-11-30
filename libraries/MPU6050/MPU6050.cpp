#include <Arduino.h>
#include "config.h"

#include "MPU6050.h"

#include "I2C.h"
#include "Endian.h"

// ================================ Public ================================ //

void MPU6050::init()
{
  uint8_t i2c_error;

  // reset device
  uint8_t reset = 0b10000000; // bit 7: reset
  #ifdef MPU6050_DEBUG
    Serial.println("MPU6050: Resetting");
  #endif
  do
  {
    i2c_error = I2C::write(MPU6050_ADDRESS, MPU6050_CONFIG, &reset, 1);
    #ifdef MPU6050_DEBUG
      Serial.print("MPU6050: Status = ");
      Serial.println(i2c_error);
    #endif
  }
  while (i2c_error != 0);
  delay(50); // give device time to reset

  // disable sleep mode
  uint8_t wakeup = 0b00000000; // bit 6: sleep mode
  #ifdef MPU6050_DEBUG
    Serial.println("MPU6050: Waking up");
  #endif
  do
  {
    i2c_error = I2C::write(MPU6050_ADDRESS, MPU6050_CONFIG, &wakeup, 1);
    #ifdef MPU6050_DEBUG
      Serial.print("MPU6050: Status = ");
      Serial.println(i2c_error);
    #endif
  }
  while (i2c_error != 0);
  delay(50); // give device time to wake up

  uint8_t scale = 0b00011000; // bit 4-3: scale
  #ifdef MPU6050_DEBUG
    Serial.println("MPU6050: Setting gyro scale");
  #endif
  do
  {
    i2c_error = I2C::write(MPU6050_ADDRESS, MPU6050_SCALE, &scale, 1);
    #ifdef MPU6050_DEBUG
      Serial.print("MPU6050: Status = ");
      Serial.println(i2c_error);
    #endif
  }
  while (i2c_error != 0);
}

void MPU6050::refresh()
{
  // read the raw data
  if(readRawData() != 0) return;

  // compute the accelerometer data
  accelX = rawData.accelX / MPU6050_ACCEL_SENSITIVITY;
  accelY = rawData.accelY / MPU6050_ACCEL_SENSITIVITY;
  accelZ = rawData.accelZ / MPU6050_ACCEL_SENSITIVITY;

  // compute the gyroscope data
  gyroX = rawData.gyroX / MPU6050_GYRO_SENSITIVITY;
  gyroY = rawData.gyroY / MPU6050_GYRO_SENSITIVITY;
  gyroZ = rawData.gyroZ / MPU6050_GYRO_SENSITIVITY;
}

// ================================ Private ================================ //

float MPU6050::accelX;
float MPU6050::accelY;
float MPU6050::accelZ;
float MPU6050::gyroX;
float MPU6050::gyroY;
float MPU6050::gyroZ;

MPU6050::rawData_t MPU6050::rawData;

uint8_t MPU6050::readRawData()
{
  uint8_t i2c_error;
  i2c_error = I2C::read(
    MPU6050_ADDRESS, MPU6050_DATA,
    (uint8_t *)&rawData, sizeof(rawData)
  );
  if(i2c_error != 0)
  {
    #ifdef MPU6050_DEBUG
      Serial.print("MPU6050: Read Error = ");
      Serial.println(i2c_error);
    #endif
    return i2c_error;
  }
  Endian::multiSwap16((uint8_t *)&rawData, 7);
  return 0;
}
