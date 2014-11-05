#include <Arduino.h>
#include "config.h"

#include <SPI.h>

#include "PS2Controller.h"

// Empirical delays
#define PS2_DELAY_AFTER_SLAVE_SELECT 7
#define PS2_DELAY_BETWEEN_WORDS 8

#define SPI_SS 10
#define SPI_MOSI 11
#define SPI_MISO 12
#define SPI_SCLK 13

#define PS2_MODE_ANALOG 0x73
#define PS2_MODE_STANDARD 0x41

// ================================ Public ================================ //

void PS2Controller::init()
{
  // Setup the SPI bus with PS2 settings
  SPI.begin();
  SPI.setDataMode(SPI_MODE3); // CPOL=1, CPHA=1
  SPI.setClockDivider(SPI_CLOCK_DIV32); // 500kHz
  SPI.setBitOrder(LSBFIRST);
}

bool PS2Controller::update()
{
  bool success = false;
  digitalWrite(SPI_SS, LOW);
  delayMicroseconds(PS2_DELAY_AFTER_SLAVE_SELECT);

  if(transferCommand(0x42))
  {
    switch(mode)
    {
      // Analog mode
      case PS2_MODE_ANALOG:
      for(uint8_t i=0; i<6; i++) status.raw[i] = transferPayload(0x00);
      success = true;
      break;

      // Standard mode
      case PS2_MODE_STANDARD:
      for(uint8_t i=0; i<2; i++) status.raw[i] = transferPayload(0x00);
      success = true;
      break;
    }
  }

  digitalWrite(SPI_SS, HIGH);
  return success;
}

bool PS2Controller::isAnalog()
{
  return (mode == PS2_MODE_ANALOG);
}

bool PS2Controller::getButton(uint16_t button)
{
  return ~status.buttons & button;
}

uint8_t PS2Controller::getAxis(uint8_t axis)
{
  return status.axis[axis];
}

// ================================ Private ================================ //

uint8_t PS2Controller::mode;
PS2Controller::status_t PS2Controller::status;

uint8_t PS2Controller::transferByte(uint8_t out)
{
  uint8_t in = SPI.transfer(out);

  #ifdef PS2_DEBUG
    Serial.print(out, HEX);
    Serial.print(":");
    Serial.println(in, HEX);
  #endif

  return in;
}

bool PS2Controller::transferCommand(uint8_t command)
{
  uint8_t response[3];

  response[0] = transferByte(0x01);
  delayMicroseconds(PS2_DELAY_BETWEEN_WORDS);
  response[1] = transferByte(command);
  delayMicroseconds(PS2_DELAY_BETWEEN_WORDS);
  response[2] = transferByte(0x00);

  if((response[0] == 0xff) && (response[2] == 0x5A))
  {
    mode = response[1];
    return true;
  }
  else
  {
    return false;
  }
}

uint8_t PS2Controller::transferPayload(uint8_t payload)
{
  delayMicroseconds(PS2_DELAY_BETWEEN_WORDS);
  return transferByte(payload);
}

