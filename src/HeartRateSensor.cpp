/**
 * @file HeartRateSensor.cpp
 * @author Josh DeWitt (jndewitt@staford.edu)
 * @brief Interface for using the MAX30102 sensor
 * @version 0.1
 * @date 2022-05-20
 * 
 * @copyright Copyright (c) 2022
 * 
 * Based on tutorial from:
 * https://lastminuteengineers.com/max30102-pulse-oximeter-heart-rate-sensor-arduino-tutorial/
 * 
 */

#include "HeartRateSensor.h"

#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

/*** function turn offs, define to enable **/
#define builtin_BeatDetect

/**** Debug prints ***/
// #define PRINT_BPM_Data 
// #define PRINT_FingerWarning
// #define PRINT_graph 

// avoid locking up on startup
#define DETECTION_BLOCKING
#define MAX_NUMTRIES    1e3
static uint16_t numtries = 0;


static MAX30105 particleSensor;

static const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
static byte rates[RATE_SIZE]; //Array of heart rates
static byte rateSpot = 0;
static long lastBeat = 0; //Time at which the last beat occurred

static float beatsPerMinute;
static int beatAvg;

static bool isFingerDetected = false; // do we have contact?

// limit rate
#define HRS_PrintPeriod    1 //[ms]
static unsigned long lastHRSprint = 0;


// function prototypes
void resetBeatValues();

void HeartRateSensor_setup(void) {

  // Initialize sensor
  while (!particleSensor.begin(Wire, I2C_SPEED_STANDARD)) {
    Serial.println("MAX30102 was not found. Please check wiring/power. ");
    #ifndef DETECTION_BLOCKING
      numtries++;
      delay(1);
      if (MAX_NUMTRIES < numtries) {
        beatAvg = 60; // tell it to just leave
        break;
      }
    #endif /* DETECTION_BLOCKING */
  }

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0x0A); //Turn off Green LED
}

void HeartRateSensor_inLoop() {
  long irValue = particleSensor.getIR();

  #ifdef PRINT_graph
    // Serial.print("IR,");
    Serial.print(particleSensor.getIR());
    // Serial.print(", ");
    // Serial.print(particleSensor.getRed());
    // Serial.print(", ");
    // Serial.print(particleSensor.getGreen());
    Serial.println();
  #endif /* PRINT_graph */

  #ifdef builtin_BeatDetect
    if (checkForBeat(irValue) == true) {
      //We sensed a beat!
      long delta = millis() - lastBeat;
      lastBeat = millis();
      Serial.println("Got a heartbeat!");

      beatsPerMinute = 60 / (delta / 1000.0);

      if (beatsPerMinute < 255 && beatsPerMinute > 20) {
        rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
        rateSpot %= RATE_SIZE; //Wrap variable

        //Take average of readings
        beatAvg = 0;
        for (byte x = 0 ; x < RATE_SIZE ; x++)
          beatAvg += rates[x];
        beatAvg /= RATE_SIZE;
      }
    }

    if(HRS_PrintPeriod < (millis() - lastHRSprint)) {
      #ifdef PRINT_BPM_Data
        Serial.print("IR=");
        Serial.print(irValue);
        Serial.print(", BPM=");
        Serial.print(beatsPerMinute);
        Serial.print(", Avg BPM=");
        Serial.println(beatAvg);
      #endif /* PRINT_BPM_Data */

      lastHRSprint = millis();
    }
  #endif /* builtin_BeatDetect */

  if (irValue < 50000) {
    isFingerDetected = false;
    // reset values!
    resetBeatValues();
    #ifdef PRINT_FingerWarning
      Serial.println(" No finger?");
    #endif /* PRINT_FingerWarning */
  } else {
    // finger is probably there?
    isFingerDetected = true;
  }

}

int getCurrentAvgBPM() {
  return beatAvg;
}

bool fingerDetected() {
  return isFingerDetected;
}

void resetBeatValues() {
  // saved variables
  beatsPerMinute = 0;
  beatAvg = 0;
  lastBeat = 0; // for time tracking
  for (uint8_t i = 0; i < RATE_SIZE; i++) {
    rates[i] = 0; // clear array
  }
}