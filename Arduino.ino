#include <Arduino_FreeRTOS.h>

bool alerting = false;
unsigned long timer = 0;
const int ledPin = 3;

void setup() {
  Serial.begin(9600);

  while (!Serial);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);

  xTaskCreate(readBreathingRate, "Breathing Rate", 128, NULL, 1, NULL);
  xTaskCreate(buzzerAlert, "Buzzer Alert", 128, NULL, 1, NULL);
  xTaskCreate(readSwitch, "Switch", 128, NULL, 1, NULL);
}

void loop() {}

void readBreathingRate(void *pvParameters) {
  const int breathingPin = A7;
  const int breathingRawAmplifiedPin = A2;
  pinMode(breathingPin, INPUT);
  pinMode(breathingRawAmplifiedPin, INPUT);

  int minnum = 500, maxnum = 0, avg;

  for (int i = 0; i < 50; i++) {
    int reading = analogRead(breathingPin);

    if (reading < minnum) {
      minnum = reading;
    }
    if (reading > maxnum) {
      maxnum = reading;
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
  avg = (minnum + maxnum) / 2;

  Serial.println("\nBreathingRate, BreathingRateRaw, Average");

  bool isHigh = false;

  while (1) {
    int breathingRate = analogRead(breathingPin);
    int breathingRawAmplifiedRate = analogRead(breathingRawAmplifiedPin);

    if (breathingRate >= avg && !isHigh || breathingRate <= avg && isHigh) {
      timer = millis();
      isHigh = !isHigh;
    }

    if (millis() - timer >= 5000) {
      alerting = true;
    }

    Serial.println(breathingRate + String(", ") + breathingRawAmplifiedRate + String(", ") + avg);

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void buzzerAlert(void *pvParameters) {
  const int buzzerPin = 5;
  pinMode(buzzerPin, OUTPUT);

  while (1) {
    while (alerting) {
      digitalWrite(buzzerPin, HIGH);
      digitalWrite(ledPin, HIGH);
      vTaskDelay(200 / portTICK_PERIOD_MS);
      digitalWrite(buzzerPin, LOW);
      digitalWrite(ledPin, LOW);
      vTaskDelay(200 / portTICK_PERIOD_MS);
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void readSwitch(void *pvParameters) {
  const int swPin = 2;
  pinMode(swPin, INPUT_PULLUP);

  bool reading = HIGH;
  bool lastState = HIGH;

  while (1) {
    reading = digitalRead(swPin);
    if (reading != lastState) {
      if (reading == LOW) {
        alerting = !alerting;
        timer = millis();
        digitalWrite(ledPin, alerting);
      }
    }
    lastState = reading;

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
