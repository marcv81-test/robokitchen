#include "config.h"

#include "Servo.h"

void setup()
{
  Servo::init();

  Servo::setChannel(0, 1000);
  Servo::setChannel(1, 1500);
}

void loop()
{
  Servo::setChannel(0, 1000);
  Servo::setChannel(0, 1200);
  Servo::setChannel(0, 1400);
  Servo::setChannel(0, 1600);
  Servo::setChannel(0, 1800);
  Servo::setChannel(0, 2000);
}
