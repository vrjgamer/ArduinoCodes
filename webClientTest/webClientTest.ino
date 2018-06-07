//JSON Library
#include <ArduinoJson.h>
//Ethernet Library
#include <SPI.h>
#include <Ethernet.h>

//board configuration
#define CONNECTION_LED 2 //connection led blue
#define STATE_LED 4 //state led green
#define POWER_LED 13 // power led onboard led

//server configuration
#define SERVER_TOKEN "A1E-MUn4UMrySO88uTp34GmoZFDAJbMy1S"
#define DEVICE_LABEL "smartswitch"
#define VARIABLE_ID_SWITCH "599d278776254214be01b35b"
#define VARIABLE_LABLE_SWITCH "switch"
#define SERVER_URL "things.ubidots.com"

//Ethernet configuration
byte mac[] = {  0x98, 0x4F, 0xEE, 0x02, 0xFF, 0x45 };
EthernetClient ethernetClient;

//Program Variables
int isplugOn = 0;
int currentState = 0;
DynamicJsonBuffer jsonBuffer(512);


//setup function to initialize
void setup() {
  //initializing LED connection states
  pinMode(CONNECTION_LED, OUTPUT);
  pinMode(STATE_LED, OUTPUT);
  pinMode(POWER_LED, OUTPUT);
  blinkLED(POWER_LED, 200);

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  digitalWrite(CONNECTION_LED, LOW);
  while (!Serial) {
    blinkLED(POWER_LED, 200);
  }

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    while (true) {
      blinkLED(POWER_LED, 200);
    }
  }


  Serial.println("");
  system("ifup eth0");
  // give the Ethernet shield a second to initialize:
  delay(1000);
  switchLEDOn(POWER_LED);

  IPAddress myIP = Ethernet.localIP();
  Serial.print("Device Local IP Address: ");
  Serial.println(myIP);
  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (ethernetClient.connect(SERVER_URL, 80)) {
    Serial.println("connected");
    switchLEDOn(CONNECTION_LED);
  }
  else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
    while (true) {
      blinkLED(CONNECTION_LED, 200);
      reconnect();
    }
  }

}

void loop() {
  // if the server's disconnected, stop the client:
  if (!ethernetClient.connected()) {
    Serial.println();
    Serial.println("disconnected!!!");
    ethernetClient.stop();
    // do nothing:
    while (!ethernetClient.connected()) {
      blinkLED(CONNECTION_LED, 200);
      reconnect();
    }
    return;
  } else {
    ethernetClient.print(F("GET /api/v1.6/variables/"VARIABLE_ID_SWITCH"/?token="));
    ethernetClient.print(SERVER_TOKEN);
    ethernetClient.print(F("&page_size="));
    ethernetClient.print(1);
    ethernetClient.println(F(" HTTP/1.1"));
    ethernetClient.println(F("Content-Type: application/json"));
    ethernetClient.print(F("Host: "));
    ethernetClient.println(SERVER_URL);
    ethernetClient.println();
  }

  String response = "";
  switchLEDOn(CONNECTION_LED);
  bool flag = false;
  int i = 0;
  while (ethernetClient.available()) {
    char c = ethernetClient.read();
    if (i == 11) {
      flag = true;
    }
    if (c == '\n' && !flag) {
      i++;
    }
    if (flag) {
      response += c;
      if (c == '\n') {
        break;
      }
    }
  }
  //Serial.println(response);
  processJSONResponse(response);
  ethernetClient.flush();
  switchLEDOff(CONNECTION_LED);
  //keep system cool
  //delay(10);
}


/***Custom Methods***/
//blink LED for paticular time
void blinkLED(int led, int time) {
  switchLEDOn(led);
  delay(time);
  switchLEDOff(led);
  delay(time);
}

//switch ON LED
void switchLEDOn(int led) {
  digitalWrite(led, HIGH);
}

//switch OFF LED
void switchLEDOff(int led) {
  digitalWrite(led, LOW);
}

//decode HTML response for getting variable values
void processJSONResponse(String response) {
  int len = (response.length() + 1);
  char* res = (char*)malloc(len * sizeof(char));
  response.toCharArray(res, len);
  //Serial.println(res);
  response = "Server Response: ";
  JsonObject& root = jsonBuffer.parseObject(res);
  free(res);
  root["last_value"].printTo(response);
  int x = (int) root["last_value"]["value"];
    Serial.println(response+"\nState: "+x);
  if (currentState != x) {
    if (x == 1) {
      switchLEDOn(STATE_LED);
    } 
//    if (x == 2) {
//      blinkLED(STATE_LED, 500);
//    } 
    else {
      switchLEDOff(STATE_LED);
    }
    currentState = x;
  }
}

//try reconnecting to internet
void reconnect() {
  system("ifup eth0");
  IPAddress myIP = Ethernet.localIP();
  Serial.print("Device Local IP Address: ");
  Serial.println(myIP);
  Serial.println("connecting...");
  if (ethernetClient.connect(SERVER_URL, 80)) {
    Serial.println("connected");
    switchLEDOn(CONNECTION_LED);
  }
  else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
  delay(200);
}

