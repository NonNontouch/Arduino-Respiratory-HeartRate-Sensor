#include <Arduino_FreeRTOS.h>

bool alerting = false;
unsigned long timer;
const int ledPin = 3;
const int nodeMCUPin = 7;

void setup() {
  Serial.begin(9600);

  pinMode(ledPin, OUTPUT);
  pinMode(nodeMCUPin, OUTPUT);

  xTaskCreate(readBreathingRate, "Breathing Rate", 128, NULL, 1, NULL);
  xTaskCreate(buzzerAlert, "Buzzer Alert", 128, NULL, 1, NULL);
  xTaskCreate(readSwitch, "Switch", 128, NULL, 1, NULL);
}

void loop() {}

void readBreathingRate(void *pvParameters) {
  const int breathingPin = A7;
  pinMode(breathingPin, INPUT);

  const int breathInPin = 5;
  const int breathOutPin = 6;
  pinMode(breathInPin, OUTPUT);
  pinMode(breathOutPin, OUTPUT);

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
  timer = millis();

  while (1) {
    int breathingRate = analogRead(breathingPin);

    if (breathingRate >= avg && !isHigh || breathingRate <= avg && isHigh) {
      timer = millis();
      isHigh = !isHigh;

      digitalWrite(breathInPin, isHigh);
      digitalWrite(breathOutPin, !isHigh);
    }

    if (!alerting && millis() - timer >= 20000) {
      alerting = true;
      digitalWrite(nodeMCUPin, HIGH);
    }


    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void buzzerAlert(void *pvParameters) {
  const int buzzerPin = 4;
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
        alerting = false;
        timer = millis();
        digitalWrite(ledPin, LOW);
        digitalWrite(nodeMCUPin, LOW);
      }
    }
    lastState = reading;

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
