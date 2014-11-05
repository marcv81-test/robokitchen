#ifndef PS2_CONTROLLER_H
#define PS2_CONTROLLER_H

#include <Arduino.h>
#include "config.h"

// Buttons definitions
#define PS2_BUTTON_SELECT 0x0001
#define PS2_BUTTON_L3 0x0002
#define PS2_BUTTON_R3 0x0004
#define PS2_BUTTON_START 0x0008
#define PS2_BUTTON_UP 0x0010
#define PS2_BUTTON_RIGHT 0x0020
#define PS2_BUTTON_DOWN 0x0040
#define PS2_BUTTON_LEFT 0x0080
#define PS2_BUTTON_L2 0x0100
#define PS2_BUTTON_R2 0x0200
#define PS2_BUTTON_L1 0x0400
#define PS2_BUTTON_R1 0x0800
#define PS2_BUTTON_TRIANGLE 0x1000
#define PS2_BUTTON_CIRCLE 0x2000
#define PS2_BUTTON_CROSS 0x4000
#define PS2_BUTTON_SQUARE 0x8000

// Axes definition
#define PS2_AXIS_RIGHT_HORIZONTAL 0
#define PS2_AXIS_RIGHT_VERTICAL 1
#define PS2_AXIS_LEFT_HORIZONTAL 2
#define PS2_AXIS_LEFT_VERTICAL 3

/*!
 * This class helps to communicate with a PS2 controller connected to the
 * SPI bus. A total of 16 buttons and 4 analog joystick axes are available.
 */
class PS2Controller
{
  public:

    /*! Initialises the SPI bus. */
    static void init();

    /*!
     * This function polls the controller for its status. Subsequent calls
     * to obtain the buttons or joysticks status are only valid if this
     * function returns true.
     */
    static bool update();

    /*!
     * This function returns whether the controller is in analog mode or not.
     * Calls to obtain joystick axes or joystick buttons status are only valid
     * if this function returns true.
     */
    static bool isAnalog();

    static bool getButton(uint16_t button);
    static uint8_t getAxis(uint8_t axis);

  private:

    // Type to store the buttons/joysticks status
    typedef union {
      struct {
        uint16_t buttons;
        uint8_t axis[4];
      };
      uint8_t raw[6];
    } status_t;

    static uint8_t mode;
    static status_t status;

    static uint8_t transferByte(uint8_t out);
    static bool transferCommand(uint8_t command);
    static uint8_t transferPayload(uint8_t payload);
};

#endif // PS2_CONTROLLER_H

