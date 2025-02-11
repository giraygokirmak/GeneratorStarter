#include <Arduino.h>

// Define the pin numbers for the onOffSwitch, choke, ignition, and contactor
const int onOffSwitchPin = 16; 
const int chokePin = 14; 
const int ignitionPin = 12; 
const int contactorPin = 13;  
const int buttonPin = 15; 

// Define the relay states and timings
bool onOffSwitchState = LOW;
bool chokeState = LOW;     
bool ignitionState = LOW;  
bool contactorState = LOW;  

unsigned long startTime;
unsigned long chokeOnTime = 10000;    // Stay on for 10 seconds total
unsigned long ignitionOnTime = 3000;  // Ignition stays on for 3 seconds
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
  pinMode(buttonPin, INPUT); // Use internal pull-up resistor

  // Initialize Serial communication
  Serial.begin(115200);

  // Record the start time
  startTime = millis();
  buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH) {
    Serial.println("Turning on onOffSwitch.");
    digitalWrite(onOffSwitchPin, HIGH); // Keep the onOffSwitch on
    onOffSwitchState = HIGH;
    Serial.println("onOffSwitch turned on.");  

    Serial.println("Turning on choke.");
    digitalWrite(chokePin, HIGH); // Turn on the choke
    chokeState = HIGH;
    chokeTriggered = true;
    Serial.println("Choke turned on.");
  }
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
    if (onOffSwitchState == HIGH) {
      Serial.println("Turning off onOffSwitch.");
      digitalWrite(onOffSwitchPin, LOW); // Turn off the onOffSwitch
      onOffSwitchState = LOW;
      Serial.println("onOffSwitch turned off.");
    }
  } 

  // Check if it's time to turn off the choke
  if (chokeTriggered && chokeState == HIGH && currentTime - startTime >= chokeOnTime) {
    Serial.println("Turning off choke.");
    digitalWrite(chokePin, LOW); // Turn off the choke
    chokeState = LOW;
    Serial.println("Choke turned off.");
  }

  // Turn on the ignition 1500ms after boot if not already triggered
  if (!ignitionTriggered && currentTime - startTime >= ignitionTriggerDelay && buttonState == HIGH) {
    Serial.println("Turning on ignition.");
    digitalWrite(ignitionPin, HIGH); // Turn on the ignition
    ignitionState = HIGH;
    ignitionTriggered = true;
    Serial.println("Ignition turned on.");
  }

  // Turn off the ignition after 5 seconds
  if (ignitionTriggered && ignitionState == HIGH && currentTime - startTime >= (ignitionTriggerDelay + ignitionOnTime)) {
    Serial.println("Turning off ignition.");
    digitalWrite(ignitionPin, LOW); // Turn off the ignition
    ignitionState = LOW;
    Serial.println("Ignition turned off.");
  }

  // Check if it's time to turn on the contactor
  if (currentTime - startTime >= contactorOnTime && buttonState == HIGH && contactorState == LOW && onOffSwitchState == HIGH) {
    Serial.println("Turning on contactor.");
    digitalWrite(contactorPin, HIGH); // Turn on the contactor
    contactorState = HIGH;
    Serial.println("Contactor turned on.");
  } else if (buttonState == LOW && contactorState == HIGH) {
    Serial.println("Turning off contactor.");
    digitalWrite(contactorPin, LOW); // Turn off the contactor if the button is pressed
    contactorState = LOW;
    Serial.println("Contactor turned off.");
  }
  
}
