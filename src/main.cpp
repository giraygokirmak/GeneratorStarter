#include <Arduino.h>

// Define the pin numbers for the onOffSwitch, choke, ignition, and contactor
const int onOffSwitchPin = 16;
const int chokePin = 14;
const int ignitionPin = 12;
const int contactorPin = 13;
const int buttonPin = 0;

// Define the relay states and timings
bool onOffSwitchState = LOW; // On at boot
bool chokeState = HIGH;      // Normally open (off) at boot
bool ignitionState = HIGH;   // Normally open (off) at boot
bool contactorState = HIGH;  // Normally open (off) at boot

unsigned long startTime;
unsigned long chokeOnTime = 10000;    // Stay on for 10 seconds total
unsigned long ignitionOnTime = 5000;  // Ignition stays on for 5 seconds
unsigned long ignitionTriggerDelay = 1500; // Ignition triggered 1500ms after boot
unsigned long contactorOnTime = 60000; // 60 seconds after boot
unsigned long engineOffOnTime = 50000; // 50 seconds after button is pressed

bool buttonPressed = false;
bool chokeTriggered = false;
bool ignitionTriggered = false;
unsigned long buttonPressTime = 0;
int buttonState = HIGH;

void setup() {
  // Initialize the relay pins and the button pin as inputs and outputs
  pinMode(onOffSwitchPin, OUTPUT);
  pinMode(chokePin, OUTPUT);
  pinMode(ignitionPin, OUTPUT);
  pinMode(contactorPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP); // Use internal pull-up resistor

  // Initialize Serial communication
  Serial.begin(115200);

  // Ensure all relays are normally open (off) at boot
  digitalWrite(onOffSwitchPin, onOffSwitchState);
  digitalWrite(chokePin, chokeState);
  digitalWrite(ignitionPin, ignitionState);
  digitalWrite(contactorPin, contactorState);

  // Record the start time
  startTime = millis();

  Serial.println("Turning on onOffSwitch.");
  digitalWrite(onOffSwitchPin, LOW); // Keep the onOffSwitch on
  onOffSwitchState = LOW;
  Serial.println("onOffSwitch turned on.");  

  Serial.println("Turning on choke.");
  digitalWrite(chokePin, LOW); // Turn on the choke
  chokeState = LOW;
  chokeTriggered = true;
  Serial.println("Choke turned on.");

  // Serial output to indicate setup complete
  Serial.println("Setup complete.");
}

void loop() {
  unsigned long currentTime = millis();
  buttonState = digitalRead(buttonPin);

  // Check if the button is pressed to start counting engineOffOnTime
  if (buttonState == LOW && !buttonPressed) {
    buttonPressed = true;
    buttonPressTime = currentTime;
    Serial.println("Button pressed. Starting engineOffOnTime countdown.");
  }

  // Turn off the onOffSwitch 50 seconds after the button is pressed
  if (buttonPressed && (currentTime - buttonPressTime >= engineOffOnTime)) {
    if (onOffSwitchState == LOW) {
      Serial.println("Turning off onOffSwitch.");
      digitalWrite(onOffSwitchPin, HIGH); // Turn off the onOffSwitch
      onOffSwitchState = HIGH;
      Serial.println("onOffSwitch turned off.");
    }
  } 

  // Check if it's time to turn off the choke
  if (chokeTriggered && chokeState == LOW && currentTime - startTime >= chokeOnTime) {
    Serial.println("Turning off choke.");
    digitalWrite(chokePin, HIGH); // Turn off the choke
    chokeState = HIGH;
    Serial.println("Choke turned off.");
  }

  // Turn on the ignition 1500ms after boot if not already triggered
  if (!ignitionTriggered && currentTime - startTime >= ignitionTriggerDelay) {
    Serial.println("Turning on ignition.");
    digitalWrite(ignitionPin, LOW); // Turn on the ignition
    ignitionState = LOW;
    ignitionTriggered = true;
    Serial.println("Ignition turned on.");
  }

  // Turn off the ignition after 5 seconds
  if (ignitionTriggered && ignitionState == LOW && currentTime - startTime >= (ignitionTriggerDelay + ignitionOnTime)) {
    Serial.println("Turning off ignition.");
    digitalWrite(ignitionPin, HIGH); // Turn off the ignition
    ignitionState = HIGH;
    Serial.println("Ignition turned off.");
  }

  // Check if it's time to turn on the contactor
  if (currentTime - startTime >= contactorOnTime && buttonState == HIGH && contactorState == HIGH && onOffSwitchState == LOW) {
    Serial.println("Turning on contactor.");
    digitalWrite(contactorPin, LOW); // Turn on the contactor
    contactorState = LOW;
    Serial.println("Contactor turned on.");
  } else if (buttonState == LOW && contactorState == LOW) {
    Serial.println("Turning off contactor.");
    digitalWrite(contactorPin, HIGH); // Turn off the contactor if the button is pressed
    contactorState = HIGH;
    Serial.println("Contactor turned off.");
  }
  
}
