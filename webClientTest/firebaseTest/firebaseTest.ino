#include <SPI.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>

byte mac[] = {  0x98, 0x4F, 0xEE, 0x02, 0xFF, 0x45 };
String firebase_host = "https://smartplugiot-89f88.firebaseio.com/";
String data;
EthernetClient client;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    while (true);
  }

  system("ifup eth0");

  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(firebase_host, 80)) {
    Serial.println("connected");
    IPAddress my = Ethernet.localIP();
    Serial.println(my);
  }
  else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  data = "{switch:'ON'}";
  client.println(firebase_host + "/DATA/" + data);

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    // do nothing forevermore:
    while (true);
  }
}
