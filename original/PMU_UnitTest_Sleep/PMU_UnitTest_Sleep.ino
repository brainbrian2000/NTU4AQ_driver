// **** INCLUDES *****
#include "LowPower.h"

void setup()
{
  // No setup is required for this library
  Serial.begin(9600);
}

void loop()
{
  Serial.println("一秒後進入睡眠");
  delay(1000);
  Serial.println("z..Z..z.");
  // Enter power down state for 8 s with ADC and BOD module disabled
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  Serial.println("起床");
  // Do something here
  // Example: Read sensor, data logging, data transmission.
}
