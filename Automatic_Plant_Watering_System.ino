#define BLYNK_TEMPLATE_ID "TMPL3kd1SqTR6"
#define BLYNK_TEMPLATE_NAME "Plant Watering System"

#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define sensor 33
#define relayPump 4      // Water pump
#define relayFertilizer 15 // Fertilizer pump

BlynkTimer timer;

// Your Auth, SSID, Password
char auth[] = "ObV3sC6BmuswFGp3oPGelG6UfQgQY0Z_";
char ssid[] = "coco";
char pass[] = "@123#Coco";

void setup()
{
  Serial.begin(115200);
  Serial.print("Starting up.");

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  pinMode(relayPump, OUTPUT);
  digitalWrite(relayPump, HIGH);
  
  pinMode(relayFertilizer, OUTPUT);
  digitalWrite(relayFertilizer, HIGH);

  Serial.print("System Started\n");

  timer.setInterval(1000L, soilMoisture);
}

void soilMoisture()
{
  int raw = analogRead(sensor);
  int value = map(raw, 0, 4095, 0, 100);
  value = (value - 100) * -1;

  Serial.print("Moisture: ");
  Serial.print(value);
  Serial.print("%\n");

  // Update Blynk widget
  Blynk.virtualWrite(V0, value);

  // Auto control water pump
  if (value < 40) {
    digitalWrite(relayPump, LOW);
    Serial.print("Motor ON\n");

    Blynk.virtualWrite(V1, 1);
  }
  else if (value > 40) {
    digitalWrite(relayPump, HIGH);
    Serial.print("Motor OFF\n");

    Blynk.virtualWrite(V1, 0);
  }
}

BLYNK_WRITE(V1) {
  bool Relay = param.asInt();

  if (Relay == 1) {
    digitalWrite(relayPump, LOW);
    Serial.print("Motor forced ON\n");

  } else {
    digitalWrite(relayPump, HIGH);
    Serial.print("Motor forced OFF\n");

  }
}

BLYNK_WRITE(V2) { // V2 controls fertilizer pump
  bool Relay = param.asInt();

  if (Relay == 1) {
    digitalWrite(relayFertilizer, LOW);
    Serial.print("Fertilizer ON\n");

  } else {
    digitalWrite(relayFertilizer, HIGH);
    Serial.print("Fertilizer OFF\n");

  }
}

void loop()
{
  Blynk.run();
  timer.run();
}
