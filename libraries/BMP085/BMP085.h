#ifndef BMP085_H
#define BMP085_H

#include <Arduino.h>

#define BMP085_DEBUG

// I2C address
#define BMP085_ADDRESS 0x77

// Registers
#define BMP085_CALIBRATION 0xAA
#define BMP085_CONTROL 0xF4
#define BMP085_DATA 0xF6

// Oversampling (0-3)
#define BMP085_OVERSAMPLING 3

// Control register commands
#define BMP085_REQUEST_TEMPERATURE 0x2E
#define BMP085_REQUEST_PRESSURE 0x34 + (BMP085_OVERSAMPLING << 6)

/*!
 * This class provides functions to read pressure data from the Bosch BMP085.
 *
 * Call init() once, then call refresh() in the main loop to refresh the pressure
 * value. Use the public accessor to access the pressure value at any time. The
 * return value of refresh() can be used to determine if a new pressure value was
 * calculated during the last cycle.
 *
 * This class implements a finite-state machine to limit the time spent in the
 * refresh() function and allow the main program loop to run at higher frequencies. 
 */
class BMP085
{

  public:

    static void init();
    static uint8_t refresh();

    static long getPressure() { return pressure; }

  private:

    /*! This type stores eleven 16-bits calibration values. */
    typedef struct calibration_t
    {
      int16_t ac1;
      int16_t ac2;
      int16_t ac3;
      uint16_t ac4;
      uint16_t ac5;
      uint16_t ac6;
      int16_t b1;
      int16_t b2;
      int16_t mb;
      int16_t mc;
      int16_t md;
    };

    /*! This type enumerates the different internal states. */
    typedef enum state_t
    {
      REQUEST_TEMPERATURE,
      READ_TEMPERATURE,
      REQUEST_PRESSURE,
      READ_PRESSURE
    };

    static long pressure;

    static uint32_t rawTemperature;
    static uint32_t rawPressure;

    static calibration_t calibration;
    static state_t state;

    /*! @return I2C error code */
    static uint8_t readCalibration();

    /*! @return I2C error code */
    static uint8_t readRawTemperature();

    /*! @return I2C error code */
    static uint8_t readRawPressure();

    /*! @return I2C error code */
    static uint8_t readRawData(uint8_t* data, byte size);

    /*!
     * Request raw temperature or pressure data.
     *
     * @param type BMP085_REQUEST_TEMPERATURE | BMP085_REQUEST_PRESSURE
     * @return I2C error code
     */
    static uint8_t request(uint8_t type);

    /*! @return 0: data available, 1: device busy, 2: an error occurred */
    static uint8_t isBusy();

    static void calculatePressure();

};

#endif // BMP085_H
