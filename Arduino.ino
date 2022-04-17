#include <Arduino_FreeRTOS.h>
#define buzzer 9
#define switch 10
void setup() {
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT);
  pinMode(switch,INPUT_PULLUP);
  //xTaskCreate(ReadHeartrate, "Heart Rate", 100, NULL, 1, NULL);
  xTaskCreate(ReadBreathing, "Breathing", 100, NULL, 1, NULL);

}

void loop() {


}
void ReadHeartrate() {
  while (1)
  {
    delay(20);
    int Heartrate = analogRead(A2);
    //Serial.println(Heartrate);
  }

}
void ReadBreathing(void *) {
  int minnum = 500, maxnum = 0, avg, Breathingrate;
  int timer;
  for (int i = 0; i < 50; i++)
  {
    int reading = analogRead(A1);
    if (reading < minnum)
    {
      minnum = reading;
    }
    if (reading > maxnum)
    {
      maxnum = reading;
    }
    delay(100);
  }
  avg = (minnum + maxnum) / 2;

  while (1)
  {
    delay(20);
    Breathingrate = analogRead(A1);
    if ((Breathingrate >= avg && Breathingrate <= avg + 50) || (Breathingrate <= avg && Breathingrate >= avg - 50))
    {
      timer = millis();
    }
    if (millis() - timer >= 5000)
    {
      BuzzerAlert();
      Serial.println("BuzzerAlert");
      timer=millis();
    }

    //Serial.println(Breathingrate);
  }
}
void BuzzerAlert() {
  while (digitalRead(switch) == HIGH)
  {
    tone(buzzer, 1000);
  }
   noTone(buzzer);
}
