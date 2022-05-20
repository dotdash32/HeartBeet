#include <Arduino.h>
#include "HeartRateSensor.h"
#include "keepAlive.h"

// keep alive messages?
#define KeepAliveMsgs

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Initializing...");
  HeartRateSensor_setup(); // init the heart rate sensor
  delay(5);
  Serial.println("Place your index finger on the sensor with steady pressure.");
}

void loop() {
  // put your main code here, to run repeatedly:

  // delay(1); // for staability?
  HeartRateSensor_inLoop();

  #ifdef KeepAliveMsgs
    keepAliveMessages();
  #endif /* KeepAliveMsgs */
}