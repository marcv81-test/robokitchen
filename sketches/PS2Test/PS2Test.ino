#include <Arduino.h>
#include "config.h"

#include "PS2Controller.h"

#include <SPI.h> // Including this fixes a probable Arduino IDE bug

void setup()
{
  Serial.begin(115200);
  Serial.println("");
  Serial.println("---");

  PS2Controller::init();
}

void loop()
{
  #ifdef SHOW_PS2_STATSTICS
    uint8_t percentage = 0;
    uint32_t startTime = micros();
    for(uint8_t i=0; i<100; i++)
    {
      if(PS2Controller::update()) { percentage++; }
      delayMicroseconds(POST_FRAME_DELAY);
    }
    uint32_t stopTime = micros();
    Serial.print("Success rate: ");
    Serial.print(percentage);
    Serial.print("%, Average frame duration: ");
    Serial.print((stopTime - startTime) / 100 - POST_FRAME_DELAY);
    Serial.println("us");
  #endif

  #ifdef SHOW_PS2_STATUS
    if(PS2Controller::update())
    {
      Serial.print("B0=");
      Serial.print(PS2Controller::getButton(PS2_BUTTON_0));
      Serial.print(", B1=");
      Serial.print(PS2Controller::getButton(PS2_BUTTON_1));
      Serial.print(", B2=");
      Serial.print(PS2Controller::getButton(PS2_BUTTON_2));
      Serial.print(", B3=");
      Serial.print(PS2Controller::getButton(PS2_BUTTON_3));
      Serial.print(", B4=");
      Serial.print(PS2Controller::getButton(PS2_BUTTON_4));
      Serial.print(", B5=");
      Serial.print(PS2Controller::getButton(PS2_BUTTON_5));
      Serial.print(", B6=");
      Serial.print(PS2Controller::getButton(PS2_BUTTON_6));
      Serial.print(", B7=");
      Serial.print(PS2Controller::getButton(PS2_BUTTON_7));
      Serial.print(", B8=");
      Serial.print(PS2Controller::getButton(PS2_BUTTON_8));
      Serial.print(", B9=");
      Serial.print(PS2Controller::getButton(PS2_BUTTON_9));
      Serial.print(", B10=");
      Serial.print(PS2Controller::getButton(PS2_BUTTON_A));
      Serial.print(", B11=");
      Serial.print(PS2Controller::getButton(PS2_BUTTON_B));
      Serial.print(", B12=");
      Serial.print(PS2Controller::getButton(PS2_BUTTON_C));
      Serial.print(", B13=");
      Serial.print(PS2Controller::getButton(PS2_BUTTON_D));
      Serial.print(", B14=");
      Serial.print(PS2Controller::getButton(PS2_BUTTON_E));
      Serial.print(", B15=");
      Serial.print(PS2Controller::getButton(PS2_BUTTON_F));
      if(PS2Controller::isAnalog())
      {
        Serial.print(", A0=");
        Serial.print(PS2Controller::getAxis(PS2_AXIS_0));
        Serial.print(", A1=");
        Serial.print(PS2Controller::getAxis(PS2_AXIS_1));
        Serial.print(", A2=");
        Serial.print(PS2Controller::getAxis(PS2_AXIS_2));
        Serial.print(", A3=");
        Serial.print(PS2Controller::getAxis(PS2_AXIS_3));
      }
      Serial.println("");
    }
    else
    {
      Serial.println("Read error");
    }
  #endif
}

