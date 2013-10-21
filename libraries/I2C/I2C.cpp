/*
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include <Arduino.h>

#include "I2C.h"

// ================================ Public ================================ //

void I2C::begin()
{
  #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega8__) || defined(__AVR_ATmega328P__)
    // Activate internal pull-ups for TWI
    // As per note from ATmega8 manual page 167
    sbi(PORTC, 4);
    sbi(PORTC, 5);
  #else
    // Activate internal pull-ups for TWI
    // As per note from ATmega128 manual page 204
    sbi(PORTD, 0);
    sbi(PORTD, 1);
  #endif
  // Initialise TWI prescaler and bit rate
  cbi(TWSR, TWPS0);
  cbi(TWSR, TWPS1);
  TWBR = ((F_CPU / 100000) - 16) / 2;
  // Enable TWI module and ACKs
  TWCR = _BV(TWEN) | _BV(TWEA);
}

void I2C::end()
{
  TWCR = 0;
}

void I2C::timeOut(uint16_t timeOut)
{
  timeOutDelay = timeOut;
}

void I2C::setSpeed(uint8_t fast)
{
  if(!fast)
  {
    TWBR = ((F_CPU / 100000) - 16) / 2;
  }
  else
  {
    TWBR = ((F_CPU / 400000) - 16) / 2;
  }
}

void I2C::pullup(uint8_t activate)
{
  if(activate)
  {
    #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega8__) || defined(__AVR_ATmega328P__)
      // Activate internal pull-ups for TWI
      // As per note from ATmega8 manual page 167
      sbi(PORTC, 4);
      sbi(PORTC, 5);
    #else
      // Activate internal pull-ups for TWI
      // As per note from ATmega128 manual page 204
      sbi(PORTD, 0);
      sbi(PORTD, 1);
    #endif
  }
  else
  {
    #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega8__) || defined(__AVR_ATmega328P__)
      // Deactivate internal pull-ups for TWI
      // As per note from ATmega8 manual page 167
      cbi(PORTC, 4);
      cbi(PORTC, 5);
    #else
      // Deactivate internal pull-ups for TWI
      // As per note from ATmega128 manual page 204
      cbi(PORTD, 0);
      cbi(PORTD, 1);
    #endif
  }
}

/*
  Return values for new functions that use the timeOut feature
  will now return at what point in the transmission the timeout
  occurred. Looking at a full communication sequence between a
  master and slave (transmit data and then readback data) there
  a total of 7 points in the sequence where a timeout can occur.
  These are listed below and correspond to the returned value:
  1 - Waiting for successful completion of a Start bit
  2 - Waiting for ACK/NACK while addressing slave in transmit mode (MT)
  3 - Waiting for ACK/NACK while sending data to the slave
  4 - Waiting for successful completion of a Repeated Start
  5 - Waiting for ACK/NACK while addressing slave in receiver mode (MR)
  6 - Waiting for ACK/NACK while receiving data from the slave
  7 - Waiting for successful completion of the Stop bit

  All possible return values:
  0           Function executed with no errors
  1 - 7       Timeout occurred, see above list
  8 - 0xFF    See datasheet for exact meaning
*/

uint8_t I2C::write(uint8_t address, uint8_t registerAddress, uint8_t *data, uint8_t numberBytes)
{
  returnStatus = 0;
  returnStatus = start();
  if(returnStatus) { return returnStatus; }
  returnStatus = sendAddress(SLA_W(address));
  if(returnStatus)
  {
    if(returnStatus == 1) { return 2; }
    return returnStatus;
  }
  returnStatus = sendByte(registerAddress);
  if(returnStatus)
  {
    if(returnStatus == 1) { return 3; }
    return returnStatus;
  }
  for(uint8_t i=0; i<numberBytes; i++)
  {
    returnStatus = sendByte(data[i]);
    if(returnStatus)
    {
      if(returnStatus == 1) { return 3; }
      return returnStatus;
    }
  }
  returnStatus = stop();
  if(returnStatus)
  {
    if(returnStatus == 1) { return 7; }
    return returnStatus;
  }
  return returnStatus;
}

uint8_t I2C::read(uint8_t address, uint8_t registerAddress, uint8_t *dataBuffer, uint8_t numberBytes)
{
  if(numberBytes == 0) { numberBytes++; }
  nack = numberBytes - 1;
  returnStatus = 0;
  returnStatus = start();
  if(returnStatus) { return returnStatus; }
  returnStatus = sendAddress(SLA_W(address));
  if(returnStatus)
  {
    if(returnStatus == 1) { return 2; }
    return returnStatus;
  }
  returnStatus = sendByte(registerAddress);
  if(returnStatus)
  {
    if(returnStatus == 1) { return 3; }
    return returnStatus;
  }
  returnStatus = start();
  if(returnStatus)
  {
    if(returnStatus == 1) { return 4; }
    return returnStatus;
  }
  returnStatus = sendAddress(SLA_R(address));
  if(returnStatus)
  {
    if(returnStatus == 1) { return 5; }
    return returnStatus;
  }
  for(uint8_t i=0; i<numberBytes; i++)
  {
    if(i == nack)
    {
      returnStatus = receiveByte(0);
      if(returnStatus == 1) { return 6; }
      if(returnStatus != MR_DATA_NACK) { return returnStatus; }
    }
    else
    {
      returnStatus = receiveByte(1);
      if(returnStatus == 1) { return 6; }
      if(returnStatus != MR_DATA_ACK) { return returnStatus; }
    }
    dataBuffer[i] = TWDR;
  }
  returnStatus = stop();
  if(returnStatus)
  {
    if(returnStatus == 1) { return 7; }
    return returnStatus;
  }
  return returnStatus;
}

// ================================ Private ================================ //

uint8_t I2C::returnStatus;
uint8_t I2C::nack;
uint16_t I2C::timeOutDelay = 0;

uint8_t I2C::start()
{
  unsigned long startingTime = millis();
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)))
  {
    if(!timeOutDelay) { continue; }
    if((millis() - startingTime) >= timeOutDelay)
    {
      lockUp();
      return 1;
    }
  }
  if ((TWI_STATUS == START) || (TWI_STATUS == REPEATED_START))
  {
    return 0;
  }
  if (TWI_STATUS == LOST_ARBTRTN)
  {
    uint8_t bufferedStatus = TWI_STATUS;
    lockUp();
    return bufferedStatus;
  }
  return TWI_STATUS;
}

uint8_t I2C::sendAddress(uint8_t i2cAddress)
{
  TWDR = i2cAddress;
  unsigned long startingTime = millis();
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)))
  {
    if(!timeOutDelay) { continue; }
    if((millis() - startingTime) >= timeOutDelay)
    {
      lockUp();
      return 1;
    }
  }
  if ((TWI_STATUS == MT_SLA_ACK) || (TWI_STATUS == MR_SLA_ACK))
  {
    return 0;
  }
  uint8_t bufferedStatus = TWI_STATUS;
  if ((TWI_STATUS == MT_SLA_NACK) || (TWI_STATUS == MR_SLA_NACK))
  {
    stop();
    return bufferedStatus;
  }
  else
  {
    lockUp();
    return bufferedStatus;
  }
}

uint8_t I2C::sendByte(uint8_t i2cData)
{
  TWDR = i2cData;
  unsigned long startingTime = millis();
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)))
  {
    if(!timeOutDelay) { continue; }
    if((millis() - startingTime) >= timeOutDelay)
    {
      lockUp();
      return 1;
    }
  }
  if (TWI_STATUS == MT_DATA_ACK)
  {
    return 0;
  }
  uint8_t bufferedStatus = TWI_STATUS;
  if (TWI_STATUS == MT_DATA_NACK)
  {
    stop();
    return bufferedStatus;
  }
  else
  {
    lockUp();
    return bufferedStatus;
  }
}

uint8_t I2C::receiveByte(uint8_t ack)
{
  unsigned long startingTime = millis();
  if(ack)
  {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
  }
  else
  {
    TWCR = (1 << TWINT) | (1 << TWEN);
  }
  while (!(TWCR & (1 << TWINT)))
  {
    if(!timeOutDelay) { continue; }
    if((millis() - startingTime) >= timeOutDelay)
    {
      lockUp();
      return 1;
    }
  }
  if (TWI_STATUS == LOST_ARBTRTN)
  {
    uint8_t bufferedStatus = TWI_STATUS;
    lockUp();
    return bufferedStatus;
  }
  return TWI_STATUS;
}

uint8_t I2C::stop()
{
  unsigned long startingTime = millis();
  TWCR = (1 << TWINT) | (1 << TWEN)| (1 << TWSTO);
  while ((TWCR & (1 << TWSTO)))
  {
    if(!timeOutDelay) { continue; }
    if((millis() - startingTime) >= timeOutDelay)
    {
      lockUp();
      return 1;
    }
  }
  return 0;
}

void I2C::lockUp()
{
  TWCR = 0; // Releases SDA and SCL lines to high impedance
  TWCR = _BV(TWEN) | _BV(TWEA); // Reinitialise TWI
}
