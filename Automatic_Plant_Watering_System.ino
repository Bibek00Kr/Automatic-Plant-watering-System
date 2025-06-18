#define BLYNK_TEMPLATE_ID "TMPL31JLvvki9"
#define BLYNK_TEMPLATE_NAME "IoT Based"
#define BLYNK_AUTH_TOKEN "LOVqfFdEpK-k0FHcxCXcwbZJZ0BEHEHh"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define sensor A0
#define relayPump 5      // GPIO5 (D1)
#define relayFertilizer 4 // GPIO4 (D2)

BlynkTimer timer;

char auth[] = "LOVqfFdEpK-k0FHcxCXcwbZJZ0BEHEHh";
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
  int value = map(raw, 0, 1023, 0, 100);
  value = (value - 100) * -1;

  Serial.print("Moisture: ");
  Serial.print(value);
  Serial.print("%\n");

  Blynk.virtualWrite(V0, value);

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

BLYNK_WRITE(V1) // Manual control for pump
{
  bool Relay = param.asInt();

  if (Relay == 1) {
    digitalWrite(relayPump, LOW);
    Serial.print("Motor forced ON\n");

  } else {
    digitalWrite(relayPump, HIGH);
    Serial.print("Motor forced OFF\n");

  }
}

BLYNK_WRITE(V2) // Manual control for fertilizer
{
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
