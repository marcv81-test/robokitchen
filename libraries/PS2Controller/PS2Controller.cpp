#include <Arduino.h>
#include "config.h"

#include <SPI.h>

#include "PS2Controller.h"

// Controller modes definition
#define PS2_MODE_STANDARD 0x40
#define PS2_MODE_ANALOG 0x70
#define PS2_MODE_CONFIG 0xF0

// Motors definition
#define PS2_MOTOR_SMALL 0
#define PS2_MOTOR_LARGE 1

// Empirical delays
#define PS2_DELAY_BETWEEN_BYTES 8
#define PS2_DELAY_BETWEEN_FRAMES 4000

// Number of updates before forced reconfigurations
#define PS2_RECONFIGURE_PERIOD 50

#define SPI_SS 10
#define SPI_MOSI 11
#define SPI_MISO 12
#define SPI_SCLK 13

// ================================ Public ================================ //

void PS2Controller::init()
{
  // Setup the SPI bus with PS2 settings
  SPI.begin();
  SPI.setDataMode(SPI_MODE3); // CPOL=1, CPHA=1
  SPI.setClockDivider(SPI_CLOCK_DIV32); // 500kHz
  SPI.setBitOrder(LSBFIRST);

  // Set TX buffer constants
  txBuffer.header[0] = 0x01;
  txBuffer.header[2] = 0x00;
}

bool PS2Controller::update()
{
  boolean status;

  // If not reconfiguring poll the controller
  if(reconfigureCounter++ < PS2_RECONFIGURE_PERIOD)
  {
    status = poll();
  }

  // Else run the reconfigure sequence then poll the controller
  else
  {
    reconfigureCounter = 0;
    status =
      enterConfig() &&
      enableAnalogMode() &&
      enableRumble() &&
      exitConfig() &&
      poll();
  }

  return status;
}

bool PS2Controller::getButton(uint16_t button)
{
  return ~rxBuffer.buttons & button;
}

uint8_t PS2Controller::getAxis(uint8_t axis)
{
  return rxBuffer.axis[axis];
}

void PS2Controller::setSmallMotor(bool on)
{
  motor[PS2_MOTOR_SMALL] = on ? 0xff : 0;
}

void PS2Controller::setLargeMotor(uint8_t speed)
{
  motor[PS2_MOTOR_LARGE] = speed;
}

// ================================ Private ================================ //

#ifdef PS2_DEBUG_STATISTICS
  uint8_t PS2Controller::transmittedFramesCounter;
  uint8_t PS2Controller::receivedFramesCounter;
  uint8_t PS2Controller::validFramesCounter;
#endif

uint8_t PS2Controller::reconfigureCounter;

uint8_t PS2Controller::motor[2];

PS2Controller::txBuffer_t PS2Controller::txBuffer;
PS2Controller::rxBuffer_t PS2Controller::rxBuffer;

bool PS2Controller::transferFrame(uint8_t length, uint8_t expectedMode)
{
  bool status = false;

  #ifdef PS2_DEBUG_STATISTICS
    transmittedFramesCounter++;
  #endif

  // Select the slave
  digitalWrite(SPI_SS, LOW);

  // Exchange the header
  for(uint8_t i=0; i<3; i++)
  {
    delayMicroseconds(PS2_DELAY_BETWEEN_BYTES);
    rxBuffer.header[i] = SPI.transfer(txBuffer.header[i]);
  }

  // If the controller set the line low for any header bit
  if((rxBuffer.header[0] & rxBuffer.header[1] & rxBuffer.header[2]) != 0xff)
  {
    #ifdef PS2_DEBUG_STATISTICS
      receivedFramesCounter++;
    #endif

    // If the controller answered with a valid header (0xff 0x?? 0x5a)
    // And the expected mode is either 0 or matches the header
    if(
      (rxBuffer.header[0] == 0xff) && (rxBuffer.header[2] == 0x5a) &&
      ((expectedMode == 0) || (expectedMode == (rxBuffer.header[1] & 0xf0)))
    )
    {
      status = true;

      #ifdef PS2_DEBUG_STATISTICS
        validFramesCounter++;
      #endif

      // Exchange the data
      for(uint8_t i=0; i<length; i++)
      {
        delayMicroseconds(PS2_DELAY_BETWEEN_BYTES);
        rxBuffer.data[i] = SPI.transfer(txBuffer.data[i]);
      }
    }
  }

  // Release the slave
  digitalWrite(SPI_SS, HIGH);

  // Wait between frames
  delayMicroseconds(PS2_DELAY_BETWEEN_FRAMES);

  // Print the frame contents
  #if PS2_DEBUG_LEVEL == 1
    if(!status)
    {
  #endif
  #if PS2_DEBUG_LEVEL >= 1
    uint8_t frameLength = 3 + (status ? length : 0);
    Serial.print("TX:");
    for(uint8_t i=0; i<frameLength; i++)
    {
      Serial.print(" ");
      if(txBuffer.raw[i] < 0x10) Serial.print("0");
      Serial.print(txBuffer.raw[i], HEX);
    }
    Serial.println();
    Serial.print("RX:");
    for(uint8_t i=0; i<frameLength; i++)
    {
      Serial.print(" ");
      if(rxBuffer.raw[i] < 0x10) Serial.print("0");
      Serial.print(rxBuffer.raw[i], HEX);
    }
    Serial.println();
  #endif
  #if PS2_DEBUG_LEVEL == 1
    }
  #endif

  // Print frames statistics
  #ifdef PS2_DEBUG_STATISTICS
    if(transmittedFramesCounter == 100)
    {
      Serial.print("Received frames: ");
      Serial.print(receivedFramesCounter);
      Serial.print("%, ");
      Serial.print("Valid frames: ");
      Serial.print(validFramesCounter);
      Serial.println("%");

      transmittedFramesCounter = 0;
      receivedFramesCounter = 0;
      validFramesCounter = 0;
    }
  #endif

  return status;
}

bool PS2Controller::poll()
{
  txBuffer.header[1] = 0x42;

  txBuffer.data[0] = motor[0];
  txBuffer.data[1] = motor[1];
  txBuffer.data[2] = 0x00;
  txBuffer.data[3] = 0x00;
  txBuffer.data[4] = 0x00;
  txBuffer.data[5] = 0x00;

  return transferFrame(6, PS2_MODE_ANALOG);
}

bool PS2Controller::enterConfig()
{
  txBuffer.header[1] = 0x43;

  txBuffer.data[0] = 0x01;
  txBuffer.data[1] = 0x00;
  txBuffer.data[2] = 0x00;
  txBuffer.data[3] = 0x00;
  txBuffer.data[4] = 0x00;
  txBuffer.data[5] = 0x00;

  return transferFrame(6, 0);
}

bool PS2Controller::exitConfig()
{
  txBuffer.header[1] = 0x43;

  txBuffer.data[0] = 0x00;
  txBuffer.data[1] = 0x00;
  txBuffer.data[2] = 0x00;
  txBuffer.data[3] = 0x00;
  txBuffer.data[4] = 0x00;
  txBuffer.data[5] = 0x00;

  return transferFrame(6, PS2_MODE_CONFIG);
}

bool PS2Controller::enableAnalogMode()
{
  txBuffer.header[1] = 0x44;

  txBuffer.data[0] = 0x01;
  txBuffer.data[1] = 0x03;
  txBuffer.data[2] = 0x00;
  txBuffer.data[3] = 0x00;
  txBuffer.data[4] = 0x00;
  txBuffer.data[5] = 0x00;

  return transferFrame(6, PS2_MODE_CONFIG);
}

bool PS2Controller::enableRumble()
{
  txBuffer.header[1] = 0x4d;

  txBuffer.data[0] = 0x00;
  txBuffer.data[1] = 0x01;
  txBuffer.data[2] = 0xff;
  txBuffer.data[3] = 0xff;
  txBuffer.data[4] = 0xff;
  txBuffer.data[5] = 0xff;

  return transferFrame(6, PS2_MODE_CONFIG);
}

