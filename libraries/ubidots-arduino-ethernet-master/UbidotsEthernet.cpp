#include "UbidotsEthernet.h"
/**
 * Constructor.
 */
Ubidots::Ubidots(char* token){
    _token = token;
    maxValues = 5;
    currentValue = 0;
    val = (Value *)malloc(maxValues*sizeof(Value));
}
/** 
 * This function is to get value from the Ubidots API
 * @arg id the id where you will get the data
 * @return num the data that you get from the Ubidots API
 */
float Ubidots::getValue(char* id){
  float num;
  String raw;
  char reply[500];
  char response[10];
  int i = 0;
  uint8_t bodyPosinit;
  uint8_t bodyPosend;
  _client.connect(SERVER, PORT);
  if (_client.connected()){
        Serial.println(F("Geting your variable"));
        // Make a HTTP request:
        _client.print(F("GET /api/v1.6/variables/"));
        _client.print(id);
        _client.println(F("/values?page_size=1 HTTP/1.1"));
        _client.println(F("Host: things.ubidots.com"));
        _client.println(F("User-Agent: Arduino-Ethernet/1.0")); 
        _client.print(F("X-Auth-Token: "));
        _client.println(_token);
        _client.println(F("Connection: close"));
        _client.println();
    } else {
        Serial.println(F("Connection failed"));  
        currentValue = 0;  
        return NULL;
    }
    int timeout = 0;
    while (!_client.available() && timeout < 5000) {
        timeout++;
        delay(1);
    }
    while (_client.available()){
        reply[i] = _client.read();
        i++;
        if(i>=499){
          i = 0;
          break;
        }
        //Serial.write(c);
    }
    while (_client.available()){
        _client.read();
    }
    _client.flush();
    _client.stop();
    Serial.println(reply);
    char* pch = strstr(reply,"\"value\":");
    raw = String(pch);
    bodyPosinit =9+ raw.indexOf("\"value\":");
    bodyPosend = raw.indexOf(", \"timestamp\"");
    num = raw.substring(bodyPosinit,bodyPosend).toFloat();    
    return num;
}
/**
 * Add a value of variable to save
 * @arg variable_id variable id to save in a struct
 * @arg value variable value to save in a struct
 */
void Ubidots::add(char *variable_id, double value){
  (val+currentValue)->id = variable_id;
  (val+currentValue)->value_id = value;
  currentValue++;
  if(currentValue>maxValues){
    Serial.println(F("You are sending more than 5 consecutives variables, you just could send 5 variables. Then other variables will be deleted!"));
    currentValue = maxValues;
  }
}
/**
 * Send all data of all variables that you saved
 * @reutrn true upon success, false upon error.
 */
bool Ubidots::sendAll(){
    int i;
    String all;
    String str;
    all = "[";
    for(i=0; i<currentValue;){
        str = String(((val+i)->value_id),5);
        all += "{\"variable\": \"";
        all += String((val + i)->id);
        all += "\", \"value\":";
        all += str;
        all += "}";
        i++;
        if(i<currentValue){
            all += ", "; 
        }
    }
    all += "]";
    i = all.length();
    
    if (_client.connect(SERVER, PORT)){
          Serial.println(F("Posting your variables"));
          _client.println(F("POST /api/v1.6/collections/values/?force=true HTTP/1.1"));
          _client.println(F("Host: things.ubidots.com"));
          _client.println(F("User-Agent: Arduino-Ethernet/1.0"));
          _client.print(F("X-Auth-Token: "));
          _client.println(_token);
          _client.println(F("Connection: close"));
          _client.println(F("Content-Type: application/json"));
          _client.print(F("Content-Length: "));
          _client.println(String(i));
          _client.println();
          _client.println(all);
          _client.println();          
    }
    int timeout = 0;
    while (!_client.available() && timeout < 5000) {
        timeout++;
        delay(1);
    }
    while (_client.available()){
        char c = _client.read();
        Serial.write(c);
    }
    currentValue = 0;
    _client.stop();
    return true;    
}
int Ubidots::freeRam () 
{
 extern int __heap_start, *__brkval; 
 int v; 
 return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
