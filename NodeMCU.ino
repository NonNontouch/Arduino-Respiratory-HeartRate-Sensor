#include <ESP8266WiFi.h>

char ssid[] = "";
char pass[] = "";

#define TokenLine ""

String alertText = "%F0%9F%9A%A8+Sleep+apnea+detected+%F0%9F%98%B1";

unsigned long timer = 0;
const int unoPin = D1;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  pinMode(unoPin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.print("\nConnected: ");
  Serial.println(WiFi.localIP());
  notifyLine("Detector started");
}

void loop() {
  delay(2000);

  int state = digitalRead(unoPin);
  Serial.println(state);
  if (state) {
    digitalWrite(LED_BUILTIN, HIGH);
    notifyLine(alertText);
  }
}

void notifyLine(String t) {
  WiFiClientSecure client;
  client.setInsecure();
  if (!client.connect("notify-api.line.me", 443)) {
    Serial.println("Connection failed");
    return;
  }
  Serial.println("Connection successful");

  String req = "";
  req += "POST /api/notify HTTP/1.1\r\n";
  req += "Host: notify-api.line.me\r\n";
  req += "Authorization: Bearer " + String(TokenLine) + "\r\n";
  req += "Cache-Control: no-cache\r\n";
  req += "User-Agent: ESP32\r\n";
  req += "Content-Type: application/x-www-form-urlencoded\r\n";
  req += "Content-Length: " + String(String("message=" + t).length()) + "\r\n";
  req += "\r\n";
  req += "message=" + t;

  Serial.println(req);
  client.print(req);

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }
}
