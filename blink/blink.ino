
void setup() {
  // put your setup code here, to run once:
  pinmode(0, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(0, 1);
  delay(1000);
  digitalWrite(0, 0);
}
