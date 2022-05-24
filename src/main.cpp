#include <Arduino.h>
#include "HeartRateSensor.h"
#include "keepAlive.h"
#include "arduino-timer.h"
#include "vibe.h"

// keep alive messages?
#define KeepAliveMsgs

auto timer = timer_create_default();


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
  timer.tick();

  // delay(1); // for staability?
  HeartRateSensor_inLoop();

  #ifdef KeepAliveMsgs
    keepAliveMessages();
  #endif /* KeepAliveMsgs */

  // sample code for testing vibe.cpp
  #if 0
  if(checkIfBeating() == false) 
  {
    startHeartbeatVibe(timer, 1000)  // start a heartbeat that will last 1 sec (60 BPM)
  }
  #endif

  // feed watchdog!
  feedWatchdog();
}