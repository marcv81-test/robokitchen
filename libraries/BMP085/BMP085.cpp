#include <Arduino.h>

#include "BMP085.h"
#include "config.h"

#include "I2C.h"
#include "Endian.h"

// ================================ Public ================================ //

void BMP085::init()
{
  uint8_t i2c_error;

  // read calibration data
  #ifdef BMP085_DEBUG
    Serial.println("BMP085: Reading Calibration Data");
  #endif
  do
  {
    i2c_error = readCalibration();
    #ifdef BMP085_DEBUG
      Serial.print("BMP085: Status = ");
      Serial.println(i2c_error);
    #endif
  }
  while (i2c_error != 0);
}

uint8_t BMP085::refresh()
{
  uint8_t ret = 0;
  switch (state)
  {
    // Read raw temperature if available
    case READ_TEMPERATURE:
    {
      if (isBusy()) return ret;
      if (readRawTemperature() == 0)
      {
        state = REQUEST_PRESSURE; // Execute next state immediately
      }
      else return ret;
    }

    // Issue a pressure request
    case REQUEST_PRESSURE:
    {
      if (request(BMP085_REQUEST_PRESSURE) == 0)
        state = READ_PRESSURE;
      return ret;
    }

    // Read raw pressure if available
    case READ_PRESSURE:
    {
      if (isBusy()) return ret;
      if (readRawPressure() == 0)
      {
        ret = 1;
        calculatePressure();
        state = REQUEST_TEMPERATURE; // Execute next state immediately
      }
      else return ret;
    }

    // Issue a temperature request
    case REQUEST_TEMPERATURE: default:
    {
      if (request(BMP085_REQUEST_TEMPERATURE) == 0)
        state = READ_TEMPERATURE;
      return ret;
    }
  }
}

// ================================ Private ================================ //

long BMP085::pressure;

uint32_t BMP085::rawTemperature;
uint32_t BMP085::rawPressure;

BMP085::calibration_t BMP085::calibration;
BMP085::state_t BMP085::state = REQUEST_TEMPERATURE;

uint8_t BMP085::readCalibration()
{
  uint8_t i2c_error;
  i2c_error = I2C::read(
    BMP085_ADDRESS, BMP085_CALIBRATION,
    (uint8_t *)&calibration, sizeof(calibration)
  );
  if(i2c_error != 0) return i2c_error;
  Endian::multiSwap16((uint8_t *)&calibration, 11);
  return 0;
}

uint8_t BMP085::readRawTemperature()
{
  uint8_t i2c_error;
  rawTemperature = 0;

  // Read into the 2 least significant (big-endian) bytes of rawTemperature
  i2c_error = readRawData(((uint8_t*)&rawTemperature)+2, 2);

  if(i2c_error != 0) { return i2c_error; }
  Endian::swap32((uint8_t*)&rawTemperature);
  return 0;
}

uint8_t BMP085::readRawPressure()
{
  uint8_t i2c_error;
  rawPressure = 0;

  // Read into the 3 least significant (big-endian) bytes of rawPressure
  i2c_error = readRawData(((uint8_t*)&rawPressure)+1, 3);

  if(i2c_error != 0) { return i2c_error; }
  Endian::swap32((uint8_t*)&rawPressure);
  rawPressure >>= (8 - BMP085_OVERSAMPLING);
  return 0;
}

uint8_t BMP085::readRawData(uint8_t* data, byte size)
{
  uint8_t i2c_error;
  i2c_error = I2C::read(BMP085_ADDRESS, BMP085_DATA, data, size);
  if(i2c_error != 0)
  {
    #ifdef BMP085_DEBUG
      Serial.print("BMP085: Read Error = ");
      Serial.println(i2c_error);
    #endif
    return i2c_error;
  }
  return 0;
}

uint8_t BMP085::request(uint8_t command)
{
  return I2C::write(BMP085_ADDRESS, BMP085_CONTROL, &command, 1);
}

uint8_t BMP085::isBusy()
{
  uint8_t i2c_error, busy;

  // Read control register
  i2c_error = I2C::read(BMP085_ADDRESS, BMP085_CONTROL, &busy, 1);
  if (i2c_error != 0) { return 2; }

  // Return state of bit 5
  return (busy & (1 << 5)) >> 5;
}

// This function calculates the pressure from the raw temperature and pressure
// values. It implements *exactly* the formula received from Bosch Sensortech.
// Fragile, do not touch.

void BMP085::calculatePressure()
{
  int32_t x1, x2, x3, b3, b5, b6, p;
  uint32_t b4, b7;

  x1 = (
    ((int32_t)rawTemperature - (int32_t)calibration.ac6) * (int32_t)calibration.ac5
  ) >> 15;
  x2 = ((int32_t)calibration.mc << 11) / (x1 + calibration.md);
  b5 = x1 + x2;

  b6 = b5 - 4000;
  x1 = (b6 * b6) >> 12;
  x1 *= calibration.b2;
  x1 >>= 11;
  x2 = (calibration.ac2 * b6);
  x2 >>= 11;
  x3 = x1 + x2;
  b3 = (((((int32_t)calibration.ac1) * 4 + x3) << BMP085_OVERSAMPLING) + 2) >> 2;

  x1 = (calibration.ac3 * b6) >> 13;
  x2 = (calibration.b1 * ((b6 * b6) >> 12)) >> 16;
  x3 = ((x1 + x2) + 2) >> 2;
  b4 = (calibration.ac4 * (uint32_t)(x3 + 32768)) >> 15;
  b7 = ((uint32_t)(rawPressure - b3) * (50000 >> BMP085_OVERSAMPLING));
  if (b7 < 0x80000000) p = (b7 << 1) / b4;
  else p = (b7 / b4) << 1;

  x1 = p >> 8;
  x1 *= x1;
  x1 = (x1 * 3038) >> 16;
  x2 = (p * -7357) >> 16;
  p += (x1 + x2 + 3791) >> 4;

  pressure = p;
}
