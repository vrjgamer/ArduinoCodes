
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

// Set these to run example.
#define FIREBASE_HOST "smartplugiot-89f88.firebaseapp.com"
#define FIREBASE_AUTH "AIzaSyAwNXOXpUilcMvsZ9FBIG58SRz5tDWUJhA"

#define WIFI_SSID "devilhunter"
#define WIFI_PASSWORD "nerodante"


void setup() {
  Serial.begin(9600);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  Firebase.set("button", 0);

  Serial.println(Firebase.success());

}

int button = 0;

void loop() {
  button++;
  Firebase.setInt("button", button);
  Serial.print("Set: ");
  Serial.println(Firebase.success());
  delay(5000);
  button = Firebase.getInt("button");
  Serial.print("Get: " + button);
  Serial.println(Firebase.success());
  delay(5000);
}
