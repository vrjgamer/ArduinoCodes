#include <PubSubClient.h>

#include <SPI.h>
#include <Ethernet.h>

//board configuration
#define CONNECTION_LED 2
#define STATE_LED 4
#define POWER_LED 13 //change this to 7 later

//server configuration
#define SERVER_TOKEN "A1E-MUn4UMrySO88uTp34GmoZFDAJbMy1S"
#define DEVICE_LABEL "SmartSwitch"
#define VARIABLE_ID_SWITCH "599b4584c03f972ab1130d76"
#define VARIABLE_LABLE_SWITCH "switch"
#define MQTT_SERVER "things.ubidots.com"

char topic[150];


byte mac[] = {  0x98, 0x4F, 0xEE, 0x02, 0xFF, 0x45 };

EthernetClient ethernetClient;

PubSubClient client(ethernetClient);

int isplugOn = 0;


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
  if (ethernetClient.connect("www.google.co.in", 80)) {
    Serial.println("connected");
    switchLEDOn(CONNECTION_LED);
    client.setServer(MQTT_SERVER, 1883);
    client.setCallback(callback);
    sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
    client.subscribe("/v1.6/devices/"DEVICE_LABEL"/"VARIABLE_LABLE_SWITCH"/lv");
  }
  else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
    while (true) {
      blinkLED(CONNECTION_LED, 200);
    }
  }
  
}

void loop(){
   if (!client.connected()) {
      reconnect();
      client.subscribe("/v1.6/devices/"DEVICE_LABEL"/"VARIABLE_LABLE_SWITCH"/lv"); 
    }
    
    char* payload = (char *) malloc(sizeof(char) * 100);
    sprintf(payload, "{\"%s\": %d}", VARIABLE_LABLE_SWITCH, isplugOn);    
    client.publish(topic, payload);
    
    Serial.println(payload);
      
    free(payload);
    client.loop();
    delay(5000);
}

void blinkLED(int led, int time) {
  switchLEDOn(led);
  delay(time);
  switchLEDOff(led);
  delay(time);
}

void switchLEDOn(int led) {
  digitalWrite(led, HIGH);
}

void switchLEDOff(int led) {
  digitalWrite(led, LOW);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  if ((char)payload[0] == '1') {
    switchLEDOn(STATE_LED);
    isplugOn = 1;
  }
  else {
    switchLEDOff(STATE_LED);
    isplugOn = 0;
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("intel-galileo-gen2", SERVER_TOKEN, "")) {
      Serial.println("connected");
      client.subscribe("/v1.6/devices/"DEVICE_LABEL"/"VARIABLE_LABLE_SWITCH"/lv");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
}
