#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <aWOT.h>

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
unsigned long ignitionOnTime = 5000;
unsigned long contactorOnTime = 60000;

#define WIFI_SSID "corpse"
#define WIFI_PASSWORD "2gy3AEZjxS4PPL"

WiFiServer server(80);
Application app;

char expectHeader[20] {};
bool shouldRestart = false;
bool wifiConnected = false;

void index(Request &req, Response &res) {
  res.set("Content-Type", "text/html");

  res.println("<html>");
  res.println("<body>");
  res.println("  <h1>");
  res.println("    Compiled: " __DATE__ " " __TIME__);
  res.println("  </h1>");
  res.println("  <form id='form'>");
  res.println("    <input id='file' type='file'>");
  res.println("    <input type='submit' value='Send' />");
  res.println("  </form>");
  res.println("</body>");
  res.println("<script>");
  res.println("  const form = document.getElementById('form');");
  res.println("  form.onsubmit = function(e) {");
  res.println("    e.preventDefault();");
  res.println("    const body = document.getElementById('file').files[0];");
  res.println("    fetch('/update', { method: 'POST', body }).then((response) => {");
  res.println("      if (!response.ok) {");
  res.println("        return alert('File upload failed');");
  res.println("      }");
  res.println("      alert('File upload succeeded');");
  res.println("    });");
  res.println("  }");
  res.println("</script>");
  res.println("</html>");
}

void update(Request &req, Response &res) {
  int contentLength = req.left();

  if (strcmp(req.get("Expect"), "100-continue") == 0) {
    res.status(100);
  }

  if (!Update.begin(contentLength)) {
    res.status(500);
    return Update.printError(req);
  }

  unsigned long start = millis();
  while (!req.available() && millis() - start <= 5000) {}

  if (!req.available()) {
    return res.sendStatus(408);
  }

  if (Update.writeStream(req) != contentLength) {
    res.status(400);
    return Update.printError(req);
  }

  if (!Update.end(true)) {
    res.status(500);
    return Update.printError(req);
  }

  shouldRestart = true;
  res.sendStatus(204);
}

void setup() {
  // Record the start time
  startTime = millis();

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

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  app.header("Expect", expectHeader, 20);
  app.get("/", &index);
  app.post("/update", &update);
  server.begin();  

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
  if (!wifiConnected && WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.println(WiFi.localIP());
  }
  
  WiFiClient client = server.available();

  if (client.connected()) {
    app.process(&client);
    client.stop();
  }

  if (shouldRestart) {
    delay(1000);
    ESP.restart();
  }

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