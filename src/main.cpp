#include <Arduino.h>
#include "HeartRateSensor.h"
#include "keepAlive.h"
#include "arduino-timer.h"
#include "vibe.h"
#include "dataLogging.h"

// keep alive messages?
#define KeepAliveMsgs

// heart beet vibe test harness
// #define VibeTest

// Accept keyboard input for modulating perceived heartrate
// #define RateModulation
#define SCALER_RESOLUTION 0.05 // Determines step size for rate modulation scaler value

// For data logging
#define DataLogging
bool D_pressed = false; // True if "D" pressed once already to confirm deletion of SD contents


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Initializing...");

  // run setup functions here

  HeartRateSensor_setup(); // init the heart rate sensor
  setupWatchdog();
  setupVibeMotors();
  setupSD();

  delay(5);
  Serial.println("Place your index finger on the sensor with steady pressure.");

}

void loop() {
  // put your main code here, to run repeatedly:

  // Check keyboard input
  if (Serial.available() > 0) {
    char inputChar = Serial.read();
    switch (inputChar) {
      /***** DATA LOGGING *****/
      // For starting/stopping data logging
      #ifdef DataLogging
      // For starting/stopping data logging
      case ('z'): {
        Serial.println("\nCMD [z]: Start logging.");
        startLogging();
        break;
      }

      case ('x'): {
        if (D_pressed) {
          Serial.println("Cancelling deletion of SD contents.");
          D_pressed = false;
        } else {
          Serial.println("\nCMD [x]: Stop logging.");
          stopLogging();
        }
        break;
      }
      
      case ('1'): {
        Serial.println("\nCMD [1]: Test log data V1.");
        logData(millis(), 1.0);
        break;
      }

      case ('2'): {
        Serial.println("\nCMD [2]: Test log data V2.");
        logData(millis(), 2.5);
        break;
      }

      case ('p'): {
        if (isLogging()) {
          Serial.println("\n[p] Logging in progress. Try again later.");
        } else {
          Serial.println("\nCMD [p]: Printing SD contents.");
          File root = SD.open("/");
          printDirectory(root, 0);
        }
        break;
      }

      case ('r'): {
        if (isLogging()) {
          Serial.println("\n[r] Logging in progress. Try again later.");
        } else {
          Serial.println("\nCMD [r]: Reading out SD contents.");
          File root = SD.open("/");
          readDirectory(root, 0);
        }
        break;
      }

      case ('D'): {
        if (!D_pressed) {
          Serial.println("\nCMD [Dx1]: Are you sure you want to DELETE SD contents?");
          Serial.println("-- Enter [D] once more to confirm. Enter [x] to cancel.");
          D_pressed = true;
        } else {
          Serial.println("\nCMD [Dx2]: Deleting SD contents.");
          File root = SD.open("/");
          deleteDirectory(root, 0);
          D_pressed = false;
        }
        break;
      }
      #endif
      /***** RATE MODULATION *****/
      // "="/"-" for modulating scaled perceived heart rate
      #ifdef RateModulation
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
      #endif

      default: break;
    }
  }

  #if 1 /////////////////////// FOR TESTING SD_CARD DATA LOGGING
  static float HRScale = 1; // Initialize scaler to 1 (i.e. match user's HR initially)

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
  #endif
}