#include <Arduino.h>
#include "config.h"

#include "HMC5883L.h"

#include "I2C.h"
#include "Endian.h"

// ================================ Public ================================ //

void HMC5883L::init()
{
  uint8_t i2c_error;

  // write config to device
  uint8_t config[3] = {
    0b00010000, // bits 5-6: no average filter, bits 2-4: 15Hz output rate,
                // bits 0-1: no current bias
    0b00100000, // bits 5-7: gain +/- 1.3Ga, bits 0-4: must be cleared
    0b00000000  // bit 7: no high-speed I2C,  bits 0-1: continuous measurement
  };
  #ifdef HMC5883L_DEBUG
    Serial.println("HMC5883L: Configuring");
  #endif
  do
  {
    i2c_error = I2C::write(
      HMC5883L_ADDRESS, HMC5883L_CONFIG,
      config, sizeof(config)
    );
    #ifdef HMC5883L_DEBUG
      Serial.print("HMC5883L: Status = ");
      Serial.println(i2c_error);
    #endif
  }
  while (i2c_error != 0);
  delay(50); // give device time to switch configuration
}

uint8_t HMC5883L::refresh()
{
  // read the raw data
  if(readRawData() != 0) return 0;

  // test if the data has been updated
  if(!hasRawDataChanged()) return 0;

  // compute the magnetometer data
  magnetX = rawData[rawDataIndex].magnetX / HMC5883L_SENSITIVITY;
  magnetY = rawData[rawDataIndex].magnetY / HMC5883L_SENSITIVITY;
  magnetZ = rawData[rawDataIndex].magnetZ / HMC5883L_SENSITIVITY;

  // increment the raw data index
  rawDataIndex ^= 1;

  return 1;
}

// ================================ Private ================================ //

float HMC5883L::magnetX;
float HMC5883L::magnetY;
float HMC5883L::magnetZ;

HMC5883L::rawData_t HMC5883L::rawData[2];
uint8_t HMC5883L::rawDataIndex = 0;

uint8_t HMC5883L::readRawData()
{
  uint8_t i2c_error;
  i2c_error = I2C::read(
    HMC5883L_ADDRESS, HMC5883L_DATA,
    (uint8_t *)&rawData[rawDataIndex], sizeof(rawData_t)
  );
  if(i2c_error != 0)
  {
    #ifdef HMC5883L_DEBUG
      Serial.print("HMC5883L: Read Error = ");
      Serial.println(i2c_error);
    #endif
    return i2c_error;
  }
  Endian::multiSwap16((uint8_t *)&rawData[rawDataIndex], 3);
  return 0;
}

uint8_t HMC5883L::hasRawDataChanged()
{
  uint8_t previousRawDataIndex = rawDataIndex ^ 1;
  return
    (rawData[rawDataIndex].magnetX != rawData[previousRawDataIndex].magnetX) ||
    (rawData[rawDataIndex].magnetY != rawData[previousRawDataIndex].magnetY) ||
    (rawData[rawDataIndex].magnetZ != rawData[previousRawDataIndex].magnetZ);
}
