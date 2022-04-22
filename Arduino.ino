#include <Arduino_FreeRTOS.h>

bool alerting = false;
unsigned long timer = 0;

void setup() {
  Serial.begin(9600);

  xTaskCreate(readBreathingRate, "Breathing Rate", 128, NULL, 1, NULL);
  xTaskCreate(buzzerAlert, "Buzzer Alert", 128, NULL, 1, NULL);
  xTaskCreate(readSwitch, "Switch", 128, NULL, 1, NULL);
}

void loop() {}

void readBreathingRate(void *pvParameters) {
  const int breathingPin = A1;
  pinMode(breathingPin, INPUT);

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

  while (1) {
    int breathingRate = analogRead(breathingPin);

    if ((breathingRate >= avg && breathingRate <= avg + 50) || (breathingRate <= avg && breathingRate >= avg - 50)) {
      timer = millis();
    }

    if (millis() - timer >= 5000) {
      alerting = true;
    }

    Serial.println(breathingRate + String(", ") + avg);

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void buzzerAlert(void *pvParameters) {
  const int buzzerPin = 9;
  pinMode(buzzerPin, OUTPUT);

  while (1) {
    while (alerting) {
      digitalWrite(buzzerPin, HIGH);
      vTaskDelay(200 / portTICK_PERIOD_MS);
      digitalWrite(buzzerPin, LOW);
      vTaskDelay(200 / portTICK_PERIOD_MS);
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void readSwitch(void *pvParameters) {
  const int swPin = 10;
  pinMode(swPin, INPUT_PULLUP);

  const int ledPin = 3;
  pinMode(ledPin, OUTPUT);

  bool reading = HIGH;
  bool lastState = HIGH;

  while (1) {
    reading = digitalRead(swPin);
    if (reading != lastState) {
      if (reading == LOW) {
        alerting = false;
        timer = millis();
        digitalWrite(ledPin, alerting);
      }
    }
    lastState = reading;

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
