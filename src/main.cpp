#include <Arduino.h>
// Define the pin numbers for the onOffSwitch, choke, ignition, and contactor
const int onOffSwitchPin = 16;
const int chokePin = 14;
const int ignitionPin = 12;
const int contactorPin = 13;
const int buttonPin = 15;

// Define the relay states and timings
bool onOffSwitchState = LOW; // On at boot
bool chokeState = HIGH;      // Normally open (off) at boot
bool ignitionState = HIGH;   // Normally open (off) at boot
bool contactorState = HIGH;  // Normally open (off) at boot

unsigned long startTime;
unsigned long chokeOnTime = 10000;    // Stay on for 10 seconds total
unsigned long ignitionOnTime = 5000;  // Ignition stays on for 5 seconds
unsigned long chokeReadyDelay = 1500; // Ignition triggered 1500ms after boot
unsigned long contactorOnTime = 60000; // 60 seconds after boot

void setup() {
  // Initialize the relay pins and the button pin as inputs and outputs
  pinMode(onOffSwitchPin, OUTPUT);
  pinMode(chokePin, OUTPUT);
  pinMode(ignitionPin, OUTPUT);
  pinMode(contactorPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP); // Use internal pull-up resistor

  // Ensure all relays are normally open (off) at boot
  digitalWrite(onOffSwitchPin, onOffSwitchState);
  digitalWrite(chokePin, chokeState);
  digitalWrite(ignitionPin, ignitionState);
  digitalWrite(contactorPin, contactorState);

  // Record the start time
  startTime = millis();
}

void loop() {
  unsigned long currentTime = millis();

  // Check if the button is pressed to turn off the onOffSwitch
  if (digitalRead(buttonPin) == LOW) {
    digitalWrite(onOffSwitchPin, HIGH); // Turn off the onOffSwitch
    onOffSwitchState = HIGH;
  } else {
    digitalWrite(onOffSwitchPin, LOW); // Keep the onOffSwitch on
    onOffSwitchState = LOW;
  }

  // Turn on the choke at boot
  if (chokeState == HIGH && currentTime - startTime) {
    digitalWrite(chokePin, LOW); // Turn on the choke
    chokeState = LOW;
  }

  // Check if it's time to turn off the choke
  if (chokeState == LOW && currentTime - startTime >= chokeOnTime) {
    digitalWrite(chokePin, HIGH); // Turn off the choke
    chokeState = HIGH;
  }

  // Turn on the ignition 1500ms after the choke is triggered on
  if (ignitionState == HIGH && currentTime - startTime >= chokeReadyDelay) {
    digitalWrite(ignitionPin, LOW); // Turn on the ignition
    ignitionState = LOW;
  }

  // Turn off the ignition after 5 seconds
  if (ignitionState == LOW && currentTime - startTime >= (chokeReadyDelay + ignitionOnTime)) {
    digitalWrite(ignitionPin, HIGH); // Turn off the ignition
    ignitionState = HIGH;
  }

  // Check if it's time to turn on the contactor
  if (currentTime - startTime >= contactorOnTime && digitalRead(buttonPin) == HIGH) {
    digitalWrite(contactorPin, LOW); // Turn on the contactor
    contactorState = LOW;
  } else {
    digitalWrite(contactorPin, HIGH); // Turn off the contactor if the button is pressed
    contactorState = HIGH;
  }
}
