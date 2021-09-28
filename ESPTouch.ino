// ESP32 Touch Test
// Just test touch pin - Touch0 is T0 which is on GPIO 4.

#include "ESPTouch.h"

ESPTouch buttonFour(4);

int count = 0;

void setup()
{
  Serial.begin(115200);
  delay(1000); // give me time to bring up serial monitor
  Serial.println("ESP32 Touch Test");
}

void loop()
{
  if(buttonFour.read()){

    Serial.printf("Press count %d\n", ++count);
  }
}
