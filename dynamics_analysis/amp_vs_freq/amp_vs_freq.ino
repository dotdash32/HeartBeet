/**********************************************
 * amp_vs_freq is a simple script used to test a user's ability to percieve different frequencies. 
 * This data will be used to develop a bode plot-like figure with freq (x) vs amplitude required for perception (y)
 * Note that all waves will be 50% duty cycle
 * 
 * CONTROLS: '-' and '=' decrements and increments the amplitude, respectively
 *           '[' and ']' decrements and increments the frequency, respectively
 */

// #define DEBUG_DUTY                // uncomment this to plot motor command waveform to serial plotter
 #define PRINT_WAVE_PARAMS         // uncomment this to print waveform parameters (amplitude and frequency)

#define AMP_STEP 0.01   // step that we increase amplitude by to determine min perceptible thresh
#define FREQ_STEP 0.125     // [hz], resolution of frequencies we will test. Might need to change this to a smaller number?
#define MIN_AMP 0.15
#define MAX_AMP 1
#define MIN_FREQ 1

double currentAmp = MIN_AMP;            // ranges from 0 to 1
double currentFreq = MIN_FREQ;            // [hz]
unsigned long timeOfNextChange;     // for tracking when the wave should change state
bool currentState;                  // false = low, true = high
String command;                     // serial command in, one of '-', '=', '[', or ']'
int duty;                           // duty cycle commanded to motor, from 0 - 255

int pwmPin_A = 3; // PWM output pin for motor 1
int pwmPin_B = 5; // PWM output pin for motor 1

void setup() {
  Serial.begin(115200);

  // set motors to output
  pinMode(pwmPin_A, OUTPUT);  
  pinMode(pwmPin_B, OUTPUT); 

  // set motors to off
  analogWrite(pwmPin_A, 0);     
  analogWrite(pwmPin_B, 0);     

  // set initial state
  timeOfNextChange = millis() + ( (1/currentFreq)/2 ) * 1000; // currentTime + period/2 [ms]
  currentState = false;
}


void loop() {
  
  // read in serial input for freq/amp changes
  if(Serial.available())
  {
        command = Serial.readStringUntil('\n');
         
        if(command.equals("-") && (currentAmp - AMP_STEP >= MIN_AMP))
        {
            currentAmp -= AMP_STEP;
        }
        else if(command.equals("=") && (currentAmp + AMP_STEP <= MAX_AMP))
        {
            currentAmp += AMP_STEP;
        }
        else if(command.equals("[") && (currentFreq - FREQ_STEP >= MIN_FREQ))
        {
            currentFreq /= pow(10,FREQ_STEP);
            currentAmp = MIN_AMP; // reset amplitude back to 0 for the start of a new test
        }
        else if(command.equals("]")){
            currentFreq *= pow(10,FREQ_STEP); 
            currentAmp = MIN_AMP; // reset amplitude back to 0 for the start of a new test
        }
        else
        {
            Serial.println("Invalid command, or new frequency/amplitude out of range");
        }

        // print current status of waveform to screen
        #ifdef PRINT_WAVE_PARAMS 
          Serial.print("frequency = ");
          Serial.print(currentFreq);
          Serial.print(" amplitude = ");
          Serial.println(currentAmp);
        #endif
  }

  // change wave from hi to low or low to hi if enough time has passed
  static unsigned long currentTime = 0;
  currentTime = millis();
  if(currentTime >= timeOfNextChange) // if it's time to change state
  {
    // write new commands to motor
    if(currentState == 0) // if motors were off, turn them on
    {
      duty = 255. * currentAmp;
    }
    else // if motors were on, turn them off
    {
      duty = 0;
    }
    analogWrite(pwmPin_A, duty);
    analogWrite(pwmPin_B, duty);

    // toggle state and setup for next change
    currentState = !currentState;
    timeOfNextChange = currentTime + ( (1/currentFreq)/2 ) * 1000; // nextTime = currentTime + period/2 [ms]
  }

  #ifdef DEBUG_DUTY
    if(currentTime%20 == 0) // every 20 ms print duty (or some other variable) to screen 
    {
//      Serial.println(currentState);
        Serial.print("current/next: ");
        Serial.print(currentTime);
        Serial.print("/");
        Serial.print(timeOfNextChange); /////////////////

        Serial.println();
    }
  #endif

}
