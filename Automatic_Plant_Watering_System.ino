#define BLYNK_TEMPLATE_ID "TMPL3kd1SqTR6"
#define BLYNK_TEMPLATE_NAME "IndoorPlantSystem"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Hardware Configuration
#define STATUS_LED 2  // ESP32 built-in LED

// Plant Configuration ==============================================
struct Plant {
  const char* name;
  int sensorPin;
  int relayPin;
  int dryThreshold;    // Water when below this %
  int wetThreshold;    // Stop watering above this %
  int fertilizerDays;  // Days between fertilization
};

Plant plants[] = {
  { // Peace Lily (Moisture-loving)
    "Peace Lily", 
    33,   // Sensor pin
    4,    // Relay pin
    40,   // Activate pump <40%
    65,   // Stop pump >65%
    14    // Fertilize every 2 weeks
  },
  { // Snake Plant (Drought-tolerant)
    "Snake Plant",
    32,   // Sensor pin
    5,    // Relay pin
    20,   // Activate pump <20%
    40,   // Stop pump >40%
    30    // Fertilize monthly
  },
  { // Pothos (Moderate needs)
    "Golden Pothos",
    35,   // Sensor pin
    18,   // Relay pin
    30,   // Activate pump <30%
    50,   // Stop pump >50%
    21    // Fertilize every 3 weeks
  }
};

const int numPlants = sizeof(plants)/sizeof(Plant);

// Network Configuration ============================================
char auth[] = "ObV3sC6BmuswFGp3oPGelG6UfQgQY0Z_";
char ssid[] = "poco";
char pass[] = "@123#Wifi";

BlynkTimer timer;

void setup() {
  // Initialize hardware
  pinMode(STATUS_LED, OUTPUT);
  Serial.begin(115200);
  
  // Initialize plant controls
  for(int i=0; i<numPlants; i++){
    pinMode(plants[i].sensorPin, INPUT);
    pinMode(plants[i].relayPin, OUTPUT);
    digitalWrite(plants[i].relayPin, HIGH); // Start with pumps OFF
  }

  // Connection sequence
  Serial.println("\nStarting connection...");
  WiFi.begin(ssid, pass);
  
  // Visual connection progress
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    digitalWrite(STATUS_LED, !digitalRead(STATUS_LED));
    delay(500);
    Serial.print(".");
    retries++;
  }

  // Connection result
  if(WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    digitalWrite(STATUS_LED, HIGH);
    
    // Start Blynk after successful WiFi
    Blynk.config(auth, "blynk.cloud", 80);
    Blynk.connect(3000); // 3-second timeout
    if(Blynk.connected()) {
      Serial.println("Blynk connected!");
    }
  } else {
    Serial.println("\nConnection failed!");
    digitalWrite(STATUS_LED, LOW);
  }

  // Setup timers
  timer.setInterval(2000L, checkMoisture);
  timer.setInterval(3600000L, checkFertilizer);
}

void loop() {
  Blynk.run();
  timer.run();
}

// Core Functions ===================================================
void waterPlant(int plantIndex, int duration) {
  Plant &p = plants[plantIndex];
  digitalWrite(p.relayPin, LOW);
  delay(duration);
  digitalWrite(p.relayPin, HIGH);
  Serial.printf("%s watered for %dms\n", p.name, duration);
}

void checkMoisture() {
  for(int i=0; i<numPlants; i++){
    Plant &p = plants[i];
    int raw = analogRead(p.sensorPin);
    int moisture = map(raw, 4095, 0, 0, 100);
    moisture = constrain(moisture, 0, 100);

    Blynk.virtualWrite(V0 + i, moisture); // V0, V1, V2 for moisture
    
    if(moisture < p.dryThreshold) {
      waterPlant(i, 3000); // Water for 3 seconds
      Blynk.virtualWrite(V5 + i, 1); // Update pump status
    }
    else if(moisture > p.wetThreshold) {
      digitalWrite(p.relayPin, HIGH);
      Blynk.virtualWrite(V5 + i, 0);
    }
    
    Serial.printf("%s: %d%%\n", p.name, moisture);
  }
}

void checkFertilizer() {
  static unsigned long lastFertilize[numPlants] = {0};
  
  for(int i=0; i<numPlants; i++){
    if(millis() - lastFertilize[i] > plants[i].fertilizerDays * 86400000L) {
      String message = String(plants[i].name) + " needs fertilizer!";
      Blynk.logEvent("fert_reminder", message.c_str());
      lastFertilize[i] = millis();
      Serial.println(message);
    }
  }
}

// Blynk Handlers ===================================================
BLYNK_WRITE(V3) { // Manual control Peace Lily
  if(param.asInt()) waterPlant(0, 3000);
}

BLYNK_WRITE(V4) { // Manual control Snake Plant
  if(param.asInt()) waterPlant(1, 2000);
}

BLYNK_WRITE(V5) { // Manual control Pothos
  if(param.asInt()) waterPlant(2, 2500);
}

BLYNK_CONNECTED() {
  Blynk.syncVirtual(V3, V4, V5);
}
