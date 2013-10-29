#ifndef HMC5883L_H
#define HMC5883L_H

#include <Arduino.h>
#include "config.h"

// I2C address
#define HMC5883L_ADDRESS 0x1E

// Registers
#define HMC5883L_CONFIG 0x00 // 3x1 bytes
#define HMC5883L_DATA 0x03 // 3x2 bytes

// Axis sensitivity
#define HMC5883L_SENSITIVITY 1090.0 // LSB per Ga

/*!
 * This class provides functions to read 3-axis magnetometer data from the Honeywell
 * HMC5883L. Call init() once, then refresh() (expensive) in the main loop to refresh
 * the data, and use the public accessors (cheap) to access the data.
 */
class HMC5883L
{

  public:

    static void init();
    static void refresh();

    static float getMagnetX() { return magnetX; }
    static float getMagnetY() { return magnetY; }
    static float getMagnetZ() { return magnetZ; }

  private:

    /*! This type stores raw magnetometer data. */
    typedef struct rawData_t
    {
      int16_t magnetX;
      int16_t magnetZ;
      int16_t magnetY;
    };

    static float magnetX;
    static float magnetY;
    static float magnetZ;

    static rawData_t rawData;

    /*!
     * This function reads raw magnetometer data and stores the values in the rawData
     * variable.
     *
     * @return I2C error code
     */
    static uint8_t readRawData();

};

#endif // HMC5883L_H
