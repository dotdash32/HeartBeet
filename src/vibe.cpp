// vibe.cpp controls the vibration motors for the heartbeet 

// includes
#include "arduino-timer.h"
#include "vibe.h"

// defines
#define S1_AMPLITUDE 0.5 * 255.
#define S2_AMPLITUDE 0.3 * 255.
#define S1_TIME 0.25
#define BETWEEN_TIME 0.25
#define S2_TIME 0.1
#define AFTER_TIME (1.0 - (S2_TIME + BETWEEN_TIME + S1_TIME))

// private functions
void setPwmFrequency(int pin, int divisor);

// module level variables
static int pwmPin_A = 3; // PWM output pin for motor 1
static int pwmPin_B = 5; // PWM output pin for motor 2
enum HBstep_t {waiting, s1, between, s2, after};
static HBstep_t HBstate = waiting;
static float thisHeartbeatTime = 0; // [ms]
Timer<> tmr;

bool setupVibeMotors(void)
{
    setPwmFrequency(pwmPin_A, 1);
    setPwmFrequency(pwmPin_B, 1);

    pinMode(pwmPin_A, OUTPUT);
    pinMode(pwmPin_B, OUTPUT); 

    analogWrite(pwmPin_A, 0);    
    analogWrite(pwmPin_B, 0);    
}

bool startHeartbeatVibe(Timer<> timer, float commandedHeartbeatTime)
{
    bool returnVal = true; 

    // store timer object and heartbeattime in module for use with continueHeartbeatVibe later
    tmr = timer;
    thisHeartbeatTime = commandedHeartbeatTime;

    // if not already sending a beat
    if((HBstate == waiting) || (HBstate == after))
    {
        // start the vibration motors
        analogWrite(pwmPin_A, S1_AMPLITUDE);
        analogWrite(pwmPin_B, S1_AMPLITUDE);

        // start the timer
        tmr.in(BETWEEN_TIME, continueHeartbeatVibe);

        // set state to S1
        HBstate = s1;
    }
    else    
    {
        returnVal = false; // indicate cannot start another heartbeat
    }

    return returnVal;
}

bool continueHeartbeatVibe(void *opaque)
{
    // HBstate contains the state we just finished
    switch (HBstate)
    {
    case s1:
        // now in the between state, so set motors to 0 and revisit this function after BETWEEN_TIME ms
        analogWrite(pwmPin_A, 0);
        analogWrite(pwmPin_B, 0);

        tmr.in(BETWEEN_TIME, continueHeartbeatVibe);

        HBstate = between;
        break;

    case between:
        // now in the s2 state, so set motors to 0 and revisit this function after S2_TIME ms
        analogWrite(pwmPin_A, S2_AMPLITUDE);
        analogWrite(pwmPin_B, S2_AMPLITUDE);

        tmr.in(S2_TIME, continueHeartbeatVibe);

        HBstate = s2;
        break;
        
    case s2:
        // now in the after state, so set motors to 0 and revisit this function after AFTER_TIME ms
        analogWrite(pwmPin_A, 0);
        analogWrite(pwmPin_B, 0);

        tmr.in(AFTER_TIME, continueHeartbeatVibe);

        HBstate = after;

        break;

    case after: 

        // done with the heartbeat, so turn everything off and don't start any timers
        analogWrite(pwmPin_A, 0);
        analogWrite(pwmPin_B, 0);
        HBstate = waiting;
        break;
    
    default:
        break;
    }
    

    // return false so we don't repeat the same timer 
    // (we've already started the timer if necessary during the switch above)
    return false; 
}

bool checkIfBeating(void)
{
    if(HBstate == waiting)
    {
        return true;
    }
    else
    {
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS

void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if (pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch (divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if (pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if (pin == 3 || pin == 11) {
    switch (divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x7; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}