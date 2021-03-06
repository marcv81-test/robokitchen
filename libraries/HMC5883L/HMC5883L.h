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
    static uint8_t refresh();

    static float getMagnetX() { return magnetX; }
    static float getMagnetY() { return magnetY; }
    static float getMagnetZ() { return magnetZ; }

  private:

    /*! This type stores raw magnetometer data. */
    typedef struct
    {
      int16_t magnetX;
      int16_t magnetZ;
      int16_t magnetY;
    } rawData_t;

    static float magnetX;
    static float magnetY;
    static float magnetZ;

    /*!
     * We alternatively read the raw magnetometer data into two different rawData_t
     * variables. This allows hasRawDataChanged() to determine whether the data has
     * been updated or not.
     */
    static rawData_t rawData[2];

    /*! Index of the current raw magnetometer data variable */
    static uint8_t rawDataIndex;

    /*!
     * This function reads the raw magnetometer data into the currently indexed
     * rawData_t variable.
     *
     * @return I2C error code
     */
    static uint8_t readRawData();

    /*!
     * This function compares the currently indexed rawData_t variable against the
     * previous one to determine whether the data has been updated or not.
     *
     * @return 1 if the set of values is new, 0 otherwise
     */
    static uint8_t hasRawDataChanged();

};

#endif // HMC5883L_H
