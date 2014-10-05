#include "config.h"

#include "PPMRX.h"

void setup()
{
  Serial.begin(115200);
  Serial.println("");
  Serial.println("---");

  PPMRX::init();
}

void loop()
{
  PPMRX::refresh();

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

  delay(250);
}
