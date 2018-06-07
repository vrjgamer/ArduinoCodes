#define tempPin 0

unsigned int tempRead = 0;
unsigned int intmp;
float temp;
int i;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  tempRead = 0;
  // put your main code here, to run repeatedly:
  for (i = 0; i < 500; i++) {
    intmp = analogRead(tempPin);
    tempRead = tempRead + intmp;
    delay(5);
  }
  tempRead = tempRead/500;
  
  temp = (5.0 * (float)tempRead * 100) / 1024;
  
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println(" C");
}
