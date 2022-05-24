
#pragma once

#include "arduino-timer.h"

/////////////////////////////////////////////////
// setupVibeMotors prepares vibe motor pins for pwm output. Call this function in setup()
bool setupVibeMotors(void);

/////////////////////////////////////////////////
// startHeartbeatVibe makes the motors vibrate in a way that haptically emulates the
//    lub-dub vibratory experience of a heartbeat. It uses timers to start and stop the vibrations,
//    so that the vibrations are non-blocking code (we can still recieve new IR data).
//
//    INPUTS: timer object created in main, thisHeartbeatTime indiciating how long the entire heartbeat should be in ms
//    RETURNS: true if heartbeat started successfully, false otherwise
bool startHeartbeatVibe(Timer<> timer, float thisHeartbeatTime);

/////////////////////////////////////////////////
// continueHeartbeatVibe is called by timer to do the next step of the heartbeat
//    (e.g. start S1, finish S2, etc) when the timer expires. This function will also 
//    start the next heartbeat step and timer if there is more steps in the current heartbeat. 
//
bool continueHeartbeatVibe(void);

/////////////////////////////////////////////////
// checkIfBeating will check if a previously-commanded beat has been completely sent 
//    - that is, S1, between, S2, and after times have all elapsed. 
//
// RETURNS: true if in the middle of a beat, false if done beating 

bool checkIfBeating(void);
