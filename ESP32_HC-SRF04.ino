// ================================================================================
// Generic example on how to program a range finder sensor using interrupts.
// The trigger signal is delivered through a hw_timer interrupt. 
// The duration of the echo signal is measured via an external interrupt on the echo signal pin.
//
// Author:  CCarreto
// Last revision:  20/06/2021
// ================================================================================

#define TRIGGER_PIN 4                                  // Trigger pin
#define ECHO_PIN 5                                     // Echo pin

hw_timer_t * timer = NULL;                             // The timer
//portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;  

const int TIMER_INTERVAL = 50;                         // Timer interval (in uS)
const int MEASURING_INTERVAL = 200;                    // Measuring intervall (in mS)

// Numer of ticks to count corresponding to the measuring interval
const int TICK_TOTAL = MEASURING_INTERVAL * 1000 / TIMER_INTERVAL;    

volatile int triggerTimeCounter;                       // Count down counter to trigger pulse time
volatile long echoStart = 0;                           // Start time of echo pulse
volatile long echoEnd = 0;                             // End time of echo pulse
volatile long echoDuration = 0;                        // Duration of echo pulse

void IRAM_ATTR onTimer() {
  static int state = 0;                                
  
  if (!(--triggerTimeCounter)) {                       // If timout
    triggerTimeCounter = TICK_TOTAL;                   // Initialize the counter
    state = 1;                                         // and change to state 1 to initiate the trigger pulse
  }

  switch (state)                                       // State machine to delivery the trigger pulse
  {
    case 0:                                            // Normal state does nothing (waites for timeout) 
      break;

    case 1:                                            // Initiate the pulse
      digitalWrite(TRIGGER_PIN, HIGH);                 // Set the trigger output high
      state = 2;                                       // and set state to 2
      break;

    case 2:                                            // Complete the pulse
    default:
      digitalWrite(TRIGGER_PIN, LOW);                  // Set the trigger output low
      state = 0;                                       // and return to state 0
      break;
  }
}

void echoPulse() {
  switch (digitalRead(ECHO_PIN))                       // Test to see if the signal is high or low
  {
    case HIGH:                                         // If High then is the start of the echo pulse
      echoEnd = 0;                                     // Clear the end time
      echoStart = micros();                            // and save the start time
      break;

    case LOW:                                          // If low then is the end of the echo pulse
      echoEnd = micros();                              // Save the end time
      echoDuration = echoEnd - echoStart;              // and calculate the pulse duration
      break;
  }
}

void setup() {
  // Initialize serial
  Serial.begin(9600);

  // Setup pins
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIGGER_PIN, OUTPUT);

  //portENTER_CRITICAL(&timerMux);
  triggerTimeCounter = TICK_TOTAL;                    // Initialize timer counter
  //portEXIT_CRITICAL(&timerMux);

  // Setup timer
  timer = timerBegin(0, 80, true);                    // Initialize timer 0
  timerAttachInterrupt(timer, &onTimer, true);        // Attach interrupt to the timer service routine 
  timerAlarmWrite(timer, TIMER_INTERVAL, true);       // Configure the timer
  timerAlarmEnable(timer);                            // Enable the timer

  // Setup external interrupt on the echo signal pin
  attachInterrupt(digitalPinToInterrupt(ECHO_PIN), echoPulse, CHANGE); 
}

void loop() {

  // Print the distance 
  Serial.println(String(echoDuration / 58) + " cm");
}
