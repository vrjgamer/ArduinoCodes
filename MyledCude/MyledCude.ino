

int layer_1 =11;
int layer_2 = 12;
int layer_3 = 13;

int stand = 2;


void setup() {
  // put your setup code here, to run once:
  pinMode(layer_1, OUTPUT);
   pinMode(layer_2, OUTPUT);
    pinMode(layer_3, OUTPUT);
     pinMode(stand, OUTPUT);
}

void loop() {
  
     digitalWrite(layer_1, LOW); 
     digitalWrite(layer_2, HIGH); 
     digitalWrite(layer_3, HIGH); 
     digitalWrite(stand, HIGH); 
}
