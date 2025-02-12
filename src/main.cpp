#include <Arduino.h>

// Define the pin numbers for the onOffSwitch, choke, ignition, and contactor
const int onOffSwitchPin = 16; 
const int ignitionPin = 14; 
const int contactorPin = 12;  
const int mainsPin = 0; 

// Define the relay states and timings
bool onOffSwitchState = LOW;
bool ignitionState = LOW;  
bool contactorState = LOW;  
bool mainsState = HIGH;

bool contactorTriggered = false;

unsigned long startTime;
unsigned long ignitionOnTime = 3000;
unsigned long contactorOnTime = 60000;


void setup() {
  // Initialize the relay pins and the mains pin as inputs and outputs

  pinMode(onOffSwitchPin, OUTPUT);
  pinMode(ignitionPin, OUTPUT);
  pinMode(contactorPin, OUTPUT);
  pinMode(mainsPin, INPUT_PULLUP); 

  digitalWrite(onOffSwitchPin, LOW);
  digitalWrite(ignitionPin, LOW);
  digitalWrite(contactorPin, LOW);  
  
  // Initialize Serial communication
  Serial.begin(115200);

  // Record the start time
  startTime = millis();

  Serial.println("Turning on onOffSwitch.");
  digitalWrite(onOffSwitchPin, HIGH); // Keep the onOffSwitch on
  Serial.println("onOffSwitch turned on.");  

  Serial.println("Turning on ignition.");
  digitalWrite(ignitionPin, HIGH); // Turn on the ignition
  Serial.println("Ignition turned on.");

  // Serial output to indicate setup complete
  Serial.println("Setup complete.");
}


void loop() {

  onOffSwitchState = digitalRead(onOffSwitchPin);
  ignitionState = digitalRead(ignitionPin);
  contactorState = digitalRead(contactorState);
  mainsState = digitalRead(mainsPin);

  unsigned long currentTime = millis();

  if (ignitionState == HIGH && currentTime - startTime >= ignitionOnTime) {
    Serial.println("Turning off ignition.");
    digitalWrite(ignitionPin, LOW); // Turn off the ignition
    Serial.println("Ignition turned off.");
  }

  if (!contactorTriggered && currentTime - startTime >= contactorOnTime && mainsState==HIGH) {
    Serial.println("Turning on contactor.");
    digitalWrite(contactorPin, HIGH); // Turn on the contactor
    Serial.println("Contactor turned on.");
    contactorTriggered = true;
  } 

  if (contactorTriggered && mainsState==LOW) {
    Serial.println("Turning off contactor.");
    digitalWrite(contactorPin, LOW); // Turn off the contactor
    Serial.println("Contactor turned off.");
    contactorTriggered = false;
  }

}