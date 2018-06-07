const int analogInPin = A2;
int sensorValue = 0;   
int outPin = 4;

int state = 0;


void setup() {
  // put your setup code here, to run once:
   Serial.begin(9600);
  pinMode(outPin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  sensorValue = analogRead(analogInPin);
   Serial.println(sensorValue);
   if(sensorValue > 50){
      if(!state){
        digitalWrite(outPin, HIGH);
        delay(150); 
        state = 1;
      }else{
         digitalWrite(outPin, LOW);
        delay(200);
        state = 0;
      } 
   }

   
}
