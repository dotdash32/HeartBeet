#include <Arduino.h>
#include "HeartRateSensor.h"
#include "keepAlive.h"
#include "arduino-timer.h"
#include "vibe.h"

// keep alive messages?
#define KeepAliveMsgs

// heart beet vibe test harness
// #define VibeTest


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Initializing...");

  // run setup functions here

  HeartRateSensor_setup(); // init the heart rate sensor
  setupWatchdog();
  setupVibeMotors();


  delay(5);
  Serial.println("Place your index finger on the sensor with steady pressure.");

}

void loop() {
  // put your main code here, to run repeatedly:

  // tick timer (used for vibe motors)
  vibe_inLoop();

  // delay(1); // for staability?
  HeartRateSensor_inLoop();

  #ifdef KeepAliveMsgs
    keepAliveMessages();
  #endif /* KeepAliveMsgs */

  // sample code for testing vibe.cpp
  #ifdef VibeTest
    static float BPM_Des = 60.;
    if (Serial.available()) {
      String inputString = Serial.readStringUntil('\n');
      float input = inputString.toFloat();
      if (input <= 150 && input >= 30) {
        // a normal heartrate!
        BPM_Des = input; //input it out!
      }
    } 
    if(checkIfBeating() == false) 
    {
      startHeartbeatVibe(BPM_Des);  // start a heartbeat that will last 1 sec (60 BPM)
    }
  #else
    if((checkIfBeating() == false) && fingerDetected()) 
    {
      startHeartbeatVibe(getCurrentAvgBPM());  // start a heartbeat that will last 1 sec (60 BPM)
    }
  #endif /* VibeTest */

  // feed watchdog!
  feedWatchdog();
}