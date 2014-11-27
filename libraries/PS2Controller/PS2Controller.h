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

    /*! Initialises the library to communicate over SPI. */
    static void init();

    /*!
     * This function polls the controller for its status. Subsequent calls
     * to obtain the buttons or joysticks status are only valid if this
     * function returns true.
     */
    static bool update();

    static bool getButton(uint16_t button);
    static uint8_t getAxis(uint8_t axis);
    static void setSmallMotor(bool on);
    static void setLargeMotor(uint8_t speed);

  private:

    #ifdef PS2_DEBUG_STATISTICS
      static uint8_t transmittedFramesCounter; // Frames transmitted
      static uint8_t receivedFramesCounter; // Frames received
      static uint8_t validFramesCounter; // Frames received with okay header
    #endif

    /*! Internal state to send frames in the right order. */
    static uint8_t state;

    /*! System time after which we may send the next frame. */
    static uint32_t nextFrameTime;

    /*! Counter to regularly reconfigure the controller (hotplug). */
    static uint8_t reconfigureCounter;

    /*! Speed of the two rumble motors. */
    static uint8_t motor[2];

    typedef union {
      struct {
        uint8_t header[3];
        uint8_t data[6];
      };
      uint8_t raw[9];
    } txBuffer_t;

    typedef union {
      struct {
        uint8_t header[3];
        union {
          struct {
            uint16_t buttons;
            uint8_t axis[4];
          };
          uint8_t data[6];
        };
      };
      uint8_t raw[9];
    } rxBuffer_t;

    /*! Transmission buffer. */
    static txBuffer_t txBuffer;

    /*! Reception buffer. */
    static rxBuffer_t rxBuffer;

    /*!
     * Transmits a frame from txBuffer and receives the results into rxBuffer.
     * Aborts the exchange after the header if there is a communication error
     * or if the controller is not in the expected mode.
     *
     * @param length Number of data bytes in txBuffer, excluding the 3
     * header bytes
     * @param expectedMode Expected controller mode (PS2_MODE_STANDARD,
     * PS2_MODE_ANALOG or PS2_MODE_CONFIG) or 0 to skip checking the mode
     * @return Whether the frame was exchanged successfully or not.
     */
    static bool transferFrame(uint8_t length, uint8_t expectedMode);

    static bool poll();
    static bool enterConfig();
    static bool exitConfig();
    static bool enableAnalogMode();
    static bool enableRumble();
};

#endif // PS2_CONTROLLER_H

