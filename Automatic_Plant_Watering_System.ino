#define BLYNK_TEMPLATE_ID "TMPL3kd1SqTR6"
#define BLYNK_TEMPLATE_NAME "Plant Watering System"
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define sensorPin 33  // Soil moisture sensor pin
#define relayPin 4    // Relay module pin
#define tempPin 32    // Temperature sensor (NTC) analog pin

char auth[] = "ObV3sC6BmuswFGp3oPGelG6UfQgQY0Z_";
char ssid[] = "poco";
char pass[] = "@123#Wifi";

BlynkTimer timer;
int moistureThreshold = 30;  // Set threshold for automatic watering

// Function declarations
void soilMoisture();
void readTemperature();

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);  // Ensure relay is OFF at startup
  pinMode(sensorPin, INPUT);
  pinMode(tempPin, INPUT);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  Blynk.setProperty(V0, "label", "Soil Moisture");
  Blynk.setProperty(V1, "label", "Temperature");
  Blynk.virtualWrite(V0, 0);
  Blynk.virtualWrite(V1, 0);

  timer.setInterval(2000L, soilMoisture);     // Run every 2 sec
  timer.setInterval(5000L, readTemperature);  // Run every 5 sec
}

void loop() {
  Blynk.run();
  timer.run();
}

void soilMoisture() {
  int value = analogRead(sensorPin);
  int moisture = map(value, 4095, 0, 0, 100);  // Convert to percentage
  moisture = constrain(moisture, 0, 100);
  Blynk.virtualWrite(V0, moisture);
  Serial.print("Moisture: ");
  Serial.println(moisture);

  if (moisture < moistureThreshold) {
    digitalWrite(relayPin, LOW);  // Turn on pump
    Serial.println("Pump ON (Auto)");
  } else {
    digitalWrite(relayPin, HIGH);  // Turn off pump
    Serial.println("Pump OFF (Auto)");
  }
}

void readTemperature() {
  int rawValue = analogRead(tempPin);
  float voltage = rawValue * (3.3 / 4095.0);    // Convert to voltage
  float temperature = (voltage - 0.5) * 100.0;  // Simple conversion (adjust if needed)
  Blynk.virtualWrite(V1, temperature);
  Serial.print("Temperature: ");
  Serial.println(temperature);
}

BLYNK_WRITE(V2) {
  int relayState = param.asInt();
  digitalWrite(relayPin, relayState ? LOW : HIGH);
  Serial.print("Manual Relay: ");
  Serial.println(relayState ? "ON" : "OFF");
}

BLYNK_CONNECTED() {
  Blynk.syncVirtual(V2);  // Sync manual relay state
}
