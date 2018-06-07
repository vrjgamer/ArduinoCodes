int sensorPin = A4;
int ledPin;

int safeLed = GREEN_LED;
int dangerLed = RED_LED;
int warningLed = 29;

int MAX_INTENSITY = 260;
int DANGER_LEVEL = 30;
int bright = 0;

void setup() {
  Serial.begin(9600);
  pinMode(safeLed, OUTPUT);
  pinMode(dangerLed, OUTPUT);
  pinMode(warningLed, OUTPUT);
}


void loop() {
  int sensorValue = analogRead(sensorPin);
  analogWrite(ledPin, 0);
  if (sensorValue > (MAX_INTENSITY / 2)) {
    ledPin = RED_LED;
    bright = MAX_INTENSITY - sensorValue;
    if (bright < 0) {
      bright = 255;
    }
    digitalWrite(warningLed, 1);
  } else {
    ledPin = GREEN_LED;
    bright = MAX_INTENSITY - sensorValue;
    digitalWrite(warningLed, 0);
  }

  analogWrite(ledPin, bright);

  Serial.print(bright);
  Serial.print(" ");
  Serial.println(sensorValue);
  delay(100);
}
