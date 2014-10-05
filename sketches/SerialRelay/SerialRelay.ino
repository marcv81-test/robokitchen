#include <SoftwareSerial.h>

SoftwareSerial softSerial(2, 3); // RX, TX

void setup()
{
  Serial.begin(115200);
  Serial.println("");
  Serial.println("===");

  softSerial.begin(9600);
}

void loop()
{
  while (softSerial.available())
  {
    Serial.write(softSerial.read());
  }

  delay(10);

  while (Serial.available())
  {
    softSerial.write(Serial.read());
  }
}
