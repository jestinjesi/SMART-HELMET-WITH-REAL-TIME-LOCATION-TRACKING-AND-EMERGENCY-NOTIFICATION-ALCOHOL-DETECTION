//Author: Barath M, BHC

#include <SoftwareSerial.h>
#include <TinyGPSPlus.h>

// Pin definitions
#define MQ3_PIN A0
#define VIB_PIN 2
#define BUZZER_PIN 9
#define LED_PIN 10

// Threshold values
#define ALCOHOL_THRESHOLD 250  // Adjust after calibration
#define IMPACT_DEBOUNCE 2000   // 2s delay between impact detections

// GPS setup
SoftwareSerial gpsSerial(4, 3); // RX, TX for GPS
TinyGPSPlus gps;

volatile bool impactDetected = false;
unsigned long lastImpactTime = 0;

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600);

  pinMode(MQ3_PIN, INPUT);
  pinMode(VIB_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(VIB_PIN), impactISR, RISING);

  Serial.println("SMART HELMET SYSTEM INITIALIZED");
}

void loop() {
  // Read GPS data continuously
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  // Read alcohol level
  int alcoholValue = analogRead(MQ3_PIN);
  Serial.print("Alcohol Sensor: ");
  Serial.print(alcoholValue);

  if (alcoholValue > ALCOHOL_THRESHOLD) {
    Serial.println("  --> Alcohol Detected!");
    triggerAlert("ALCOHOL DETECTED!");
  } else {
    Serial.println("  --> Normal");
  }

  // Check for impact detection
  if (impactDetected && millis() - lastImpactTime > IMPACT_DEBOUNCE) {
    impactDetected = false;
    lastImpactTime = millis();
    Serial.println("⚠️  Impact Detected! Possible accident.");
    triggerAlert("IMPACT DETECTED!");
  }

  // Show GPS data
  if (gps.location.isValid()) {
    Serial.print("Location: ");
    Serial.print(gps.location.lat(), 6);
    Serial.print(", ");
    Serial.println(gps.location.lng(), 6);
  } else {
    Serial.println("GPS: Waiting for fix...");
  }

  delay(1000);
}

void impactISR() {
  impactDetected = true;
}

void triggerAlert(String reason) {
  Serial.println("==== ALERT: " + reason + " ====");
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  }
}
