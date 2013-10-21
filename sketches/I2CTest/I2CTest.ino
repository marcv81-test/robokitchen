#include <I2C.h>

#define MPU6050_ADDRESS 0x68
#define MPU6050_WHO_AM_I 0x75

uint8_t b;

void setup()
{
  I2c.begin();
  Serial.begin(9600);
  Serial.println("---");
}

void loop()
{
  I2c.read(MPU6050_ADDRESS, MPU6050_WHO_AM_I, &b, 1); // read the MPU-6050 WHO_AM_I register
  Serial.println(b, 16); // print the result
  delay(1000);
}
