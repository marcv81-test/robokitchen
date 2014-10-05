#define DEBUG 1

// sonar config
#define SONAR_TRIG_PIN 2
#define SONAR_ECHO_PIN 3
#define SONAR_ECHO_INTERRUPT 1 // interrupt 1 is on pin 3 on uno
#define SONAR_FREQUENCY 10 // hertz

// sonar variables
volatile long sonarEchoStartTime = 0;
volatile long sonarEchoStopTime = 0;
volatile boolean sonarEchoAvailable = false;
float sonarDistance = 0.0;
uint32_t sonarNextTrigger = 0;

// sonar interrupt
void sonarEchoISR()
{
  switch(digitalRead(SONAR_ECHO_PIN))
  {
    // rising transitions
    case HIGH:
      sonarEchoStartTime = micros();
    break;

    // falling transitions
    case LOW:
      sonarEchoStopTime = micros();
      sonarEchoAvailable = true;
    break;
  }
}

// sonar setup
void sonarSetup()
{
  // setup trigger pin
  pinMode(SONAR_TRIG_PIN, OUTPUT);
  digitalWrite(SONAR_TRIG_PIN, LOW);

  // setup echo pin
  pinMode(SONAR_ECHO_PIN, INPUT);
  attachInterrupt(SONAR_ECHO_INTERRUPT, sonarEchoISR, CHANGE);
}

// sonar loop
void sonarLoop()
{
  // calculate distance if available
  if(sonarEchoAvailable)
  {
    sonarDistance = 340.0 * (((sonarEchoStopTime - sonarEchoStartTime) / 1000000.0) / 2);
    if((sonarDistance > 4.0) || (sonarDistance < 0.0)) { sonarDistance = 0.0; } // distance was out of range
    sonarEchoAvailable = false;

    // display sonar distance in debug mode
    #ifdef DEBUG
      Serial.print("Sonar: ");
      Serial.println(1000.0 * sonarDistance);
    #endif
  }

  // check if ready to trigger
  if(micros() > sonarNextTrigger)
  {
    // trigger sonar
    digitalWrite(SONAR_TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(SONAR_TRIG_PIN, LOW);

    // schedule next trigger
    sonarNextTrigger = micros() + 1000000 / SONAR_FREQUENCY;
  }
}

void setup()
{
  sonarSetup();

  #ifdef DEBUG
    Serial.begin(9600);
    Serial.println("---");
  #endif
}

void loop()
{
  sonarLoop();
}

