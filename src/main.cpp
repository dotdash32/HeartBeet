#include <Arduino.h>
#include "HeartRateSensor.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Initializing...");
  HeartRateSensor_setup(); // init the heart rate sensor
  Serial.println("Place your index finger on the sensor with steady pressure.");
}

void loop() {
  // put your main code here, to run repeatedly:

  HeartRateSensor_inLoop();
}