#include <Arduino.h>
#include "HeartRateSensor.h"
#include "keepAlive.h"
#include "arduino-timer.h"
#include "vibe.h"

// keep alive messages?
#define KeepAliveMsgs

// heart beet vibe test harness
// #define VibeTest

// Accept keyboard input for modulating perceived heartrate
#define RateModulation
#define SCALER_RESOLUTION 0.05 // Determines step size for rate modulation scaler value


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
  static float HRScale = 1; // Initialize scaler to 1 (i.e. match user's HR initially)

  // tick timer (used for vibe motors)
  vibe_inLoop();

  // delay(1); // for staability?
  HeartRateSensor_inLoop();

  #ifdef KeepAliveMsgs
    keepAliveMessages();
  #endif /* KeepAliveMsgs */

  // Accept keyboard input "="/"-" for modulating scaled perceived heartrate
  #ifdef RateModulation
    if (Serial.available() > 0) {
      char inputChar = Serial.read();
      switch (inputChar) {
        // Increase scaler
        case ('='): {
          HRScale += SCALER_RESOLUTION;
          Serial.print("HR Scaler increased to ");
          Serial.println(HRScale);
          break;
        }

        // Decrease scaler if scaler will stay nonzero
        case ('-'): {
          // Do not decrease if scaler will become 0
          if (HRScale > SCALER_RESOLUTION) {
            HRScale -= SCALER_RESOLUTION;
            Serial.print("HR Scaler decreased to ");
            Serial.println(HRScale);
          }
          break;
        }

        // Reset scaler to 1
        case ('0'): {
          HRScale = 1;
          Serial.println("HR Scaler reset to 1");
          break;
        }

      }
    }
  #endif

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
      int currAvgBPM = getCurrentAvgBPM();
      
      // start a heartbeat that will last 1 sec (60 BPM)
      if (startHeartbeatVibe(HRScale * currAvgBPM)) {
        Serial.print("Current HR: ");
        Serial.print(currAvgBPM);
        Serial.print("\tCommanded HR: ");
        Serial.println(HRScale * currAvgBPM);
      }
    }
  #endif /* VibeTest */

  // feed watchdog!
  feedWatchdog();
}