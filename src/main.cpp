#include <Arduino.h>

// Define the pin numbers for the onOffSwitch, choke, ignition, and contactor
const int onOffSwitchPin = 16; 
const int ignitionPin = 12; 
const int contactorPin = 13;  
const int mainsPin = 15; 

// Define the relay states and timings
bool onOffSwitchState = LOW;
bool ignitionState = LOW;  
bool contactorState = LOW;  

unsigned long startTime;
unsigned long ignitionOnTime = 3000;  // Ignition stays on for 3s
unsigned long ignitionTriggerDelay = 1000; // Ignition triggered 1s after boot
unsigned long contactorOnTime = 60000; // 60s after boot
unsigned long engineOffOnTime = 45000; // 45s after mains is alive
unsigned long mainsOnTime = 5000; // 5s after mains is alive

bool mains = false;
bool ignitionTriggered = false;
unsigned long mainsTime = 0;
int mainsState = HIGH;

void setup() {
  // Initialize the relay pins and the mains pin as inputs and outputs
  pinMode(onOffSwitchPin, OUTPUT);
  pinMode(ignitionPin, OUTPUT);
  pinMode(contactorPin, OUTPUT);
  pinMode(mainsPin, INPUT); 

  // Initialize Serial communication
  Serial.begin(115200);

  // Record the start time
  startTime = millis();
  mainsState = digitalRead(mainsPin);

  if (mainsState == HIGH) {
    Serial.println("Turning on onOffSwitch.");
    digitalWrite(onOffSwitchPin, HIGH); // Keep the onOffSwitch on
    onOffSwitchState = HIGH;
    Serial.println("onOffSwitch turned on.");  
  }

  // Serial output to indicate setup complete
  Serial.println("Setup complete.");
}


void loop() {
  unsigned long currentTime = millis();
  mainsState = digitalRead(mainsPin);

  if (mainsState == HIGH) {
    mainsTime=0;
  }

  // Check if the mains is alive to start counting engineOffOnTime
  if (mainsState == LOW && mainsTime==0) {
    mainsTime = currentTime;
    Serial.println("Mains alive. Starting countdown.");
  }

  if (!mains && mainsState == LOW && (currentTime - mainsTime >= mainsOnTime)) { 
    mains=true;
    mainsTime = currentTime;
    Serial.println("Mains alive countdown finished. Starting engineOffOnTime countdown.");
  }

  // Turn off the onOffSwitch 50 seconds after the mains is alive
  if (mains && (currentTime - mainsTime >= engineOffOnTime)) {
    if (onOffSwitchState == HIGH) {
      Serial.println("Turning off onOffSwitch.");
      digitalWrite(onOffSwitchPin, LOW); // Turn off the onOffSwitch
      onOffSwitchState = LOW;
      Serial.println("onOffSwitch turned off.");
    }
  } 

  // Turn on the ignition 1500ms after boot if not already triggered
  if (!mains && !ignitionTriggered && currentTime - startTime >= ignitionTriggerDelay && mainsState == HIGH) {
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
  if (!mains && currentTime - startTime >= contactorOnTime && mainsState == HIGH && contactorState == LOW && onOffSwitchState == HIGH) {
    Serial.println("Turning on contactor.");
    digitalWrite(contactorPin, HIGH); // Turn on the contactor
    contactorState = HIGH;
    Serial.println("Contactor turned on.");
  } else if (mains && mainsState == LOW && contactorState == HIGH) {
    Serial.println("Turning off contactor.");
    digitalWrite(contactorPin, LOW); // Turn off the contactor if the mains is alive
    contactorState = LOW;
    Serial.println("Contactor turned off.");
  }
  
}