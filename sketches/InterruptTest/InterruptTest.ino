#include "config.h"

#include "PPMRX.h"
#include "Servo.h"

void setup()
{
  Serial.begin(115200);
  Serial.println("");
  Serial.println("---");

  Servo::init();
  PPMRX::init();
}

void loop()
{
  if(PPMRX::refresh())
  {
    Servo::setChannel(0, 1500 - (6.5 * PPMRX::getChannel(PPMRX_CHANNEL_THROTTLE)));
    Servo::setChannel(1, 1500 - (6.5 * PPMRX::getChannel(PPMRX_CHANNEL_RUDDER)));
  }

  Serial.print(PPMRX::getChannel(PPMRX_CHANNEL_THROTTLE));
  Serial.print(", ");
  Serial.print(PPMRX::getChannel(PPMRX_CHANNEL_RUDDER));
  Serial.print(", ");
  Serial.print(PPMRX::getChannel(PPMRX_CHANNEL_ELEVATOR));
  Serial.print(", ");
  Serial.print(PPMRX::getChannel(PPMRX_CHANNEL_AILERONS));
  Serial.print(", ");
  Serial.print(PPMRX::getChannel(PPMRX_CHANNEL_GEAR));
  Serial.print(", ");
  Serial.println(PPMRX::getChannel(PPMRX_CHANNEL_AUX));
}
