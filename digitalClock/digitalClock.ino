#include <ArduinoJson.h>
#include <LiquidCrystal.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

const int rs = 15, en = 13, d4 = 14, d5 = 2, d6 = 0, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int s = 0;
int h = 0;
int m = 0;
int day = 0;

String days[7] = {"Sun", "Mon", "Tue", "Wed", "Thur", "Fri", "Sat"};

WiFiClient client;
int updateCheck = 0;
int UPDATE_RETRY = 5;

String weatherMain = "";
String weatherLocation = "";
String country;
String location = "Calicut";
int humidity;
int pressure;
float temp;
float tempMin, tempMax;
int clouds;
float windSpeed;

int d_index = 0;

const char* ssid     = "devilhunter";
const char* password = "nerodante";
String weatherKey = "5ba470971810736f3de93f55b18a3cca";
String weatherLang = "&lang=en";
String cityID = "1265874";//calicut  /*"1262180"; //nagpur*/
const char *weatherHost = "api.openweathermap.org";

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.clear();
  Serial.print("Connecting WiFi ");
  lcd.print("Connecting...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected: ");
  Serial.println(WiFi.localIP());
  lcd.clear();
  clearLCDLine(0);
  lcd.print("Connected: ");
  clearLCDLine(1);
  lcd.println(WiFi.localIP());
  delay(500);
  updateDataFromInternet();
}

void clearLCDLine(int lineNum) {
  lcd.setCursor(0, lineNum);
  lcd.print("                ");
  lcd.setCursor(0, lineNum);
}

void loop() {
  s++;
  if (s >= 60) {
    s = 0;
    m++;
    if (m >= 60) {
      m = 0;
      h++;
      if(updateCheck > 0){
        updateDataFromInternet();
      }
      if (h >= 24) {
        h = 0;
        day++;
        if(day >= 7){
          day = 0;
        }
        updateDataFromInternet();
      }
    }
  }
  printTime();
}

void printTime() {
  switch (d_index) {
    case 0:
    case 1:
      clearLCDLine(0);
      lcd.print("Location:");
      lcd.print(location);
      break;
    case 2:
    case 3:
      clearLCDLine(0);
      lcd.print("Temp.:");
      lcd.print(temp);
      lcd.print(" C");
      break;
    case 4:
    case 5:
      clearLCDLine(0);
      lcd.print("MaxTemp.:");
      lcd.print(tempMax);
      lcd.print(" C");
      break;
    case 6:
    case 7:
      clearLCDLine(0);
      lcd.print("MinTemp.:");
      lcd.print(tempMin);
      lcd.print(" C");
      break;
    case 8:
    case 9:
      clearLCDLine(0);
      lcd.print("Pressure:");
      lcd.print(pressure);
      lcd.print("hPa");
      break;
    case 10:
    case 11:
      clearLCDLine(0);
      lcd.print("Humidity:");
      lcd.print(humidity);
      lcd.print("%");
      break;
    case 12:
    case 13:
      clearLCDLine(0);
      lcd.print("Clouds:");
      lcd.print(clouds);
      lcd.print("%");
      break;
    case 14:
    case 15:
      clearLCDLine(0);
      lcd.print("Wind:");
      lcd.print(windSpeed);
      lcd.print("m/s");
      break;
    case 16:
    case 17:
      clearLCDLine(0);
      lcd.print("#TimeNeverWaits");
      break;
    case 18:
    case 19:
      clearLCDLine(0);
      lcd.print("Vrushabh R. J.");
      break;
    default:
      d_index = 0;
  }
  d_index++;
  if (d_index >= 20) {
    d_index = 0;
  }
  clearLCDLine(1);
  clearLCDLine(1);
  //print day
  lcd.print(days[day]);
  lcd.print(" ");
  //print hour
  if (h == 0 || h == 12) {
    lcd.print("12");
  } else if (h > 0 && h <= 9) {
    lcd.print("0");
    lcd.print(h);
  }else if(h > 9 && h < 12){
    lcd.print(h);  
  }else {
    int hr = (h - 12);
    if (hr > 0 && hr <= 9) {
      lcd.print("0");
      lcd.print(hr);
    } else {
      lcd.print(hr);
    }
  }
  lcd.print(":");
  //print minute
  if (m >= 0 && m <= 9) {
    lcd.print("0");
    lcd.print(m);
  } else {
    lcd.print(m);
  }
  lcd.print(":");
  //print seconds
  if (s >= 0 && s <= 9) {
    lcd.print("0");
    lcd.print(s);
  } else {
    lcd.print(s);
  }
  
  if (h >= 12) {
    lcd.print(" PM");
  } else {
    lcd.print(" AM");
  }
  Serial.println(String(h) + ":" + String(m)+":"+String(s));
  delay(1000);
}

void updateDataFromInternet() {
  if (WiFi.status() == WL_CONNECTED) {
    lcd.clear();
    clearLCDLine(0);
    lcd.print("Fetching weather");
    clearLCDLine(1);
    lcd.print("Location:Calicut"); // Nagpur");
    getWeatherData();
    clearLCDLine(0);
    lcd.print("Got weather");
    delay(500);
    clearLCDLine(0);
    lcd.print("Fetching time");
    getTime();
    clearLCDLine(0);
    lcd.print("Got time");
    updateCheck = 0;
  } else {
    clearLCDLine(0);
    lcd.print("No connection: ");
    if(updateCheck == 0){
        updateCheck = UPDATE_RETRY;  
    }
    updateCheck--;
    clearLCDLine(0);
    lcd.print("Retries left: ");
    lcd.print(updateCheck);
    return;
  }
}

void getTime() {
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP, "2.in.pool.ntp.org", 3600 * 5.5); // 5.5 to set it to india
  timeClient.update();
  day = timeClient.getDay();
  h = ((int)(timeClient.getFormattedTime()[0] - '0')) * 10 + (int)(timeClient.getFormattedTime()[1] - '0');
  m = ((int)(timeClient.getFormattedTime()[3] - '0')) * 10 + (int)(timeClient.getFormattedTime()[4] - '0');
  s = ((int)(timeClient.getFormattedTime()[6] - '0')) * 10 + (int)(timeClient.getFormattedTime()[7] - '0');
  timeClient.end();
}


void getWeatherData() {
  Serial.print("connecting to ");
  Serial.println(weatherHost);
  if (client.connect(weatherHost, 80)) {
    client.println(String("GET /data/2.5/weather?id=") + cityID + "&units=metric&appid=" + weatherKey + weatherLang + "\r\n" +
                   "Host: " + weatherHost + "\r\nUser-Agent: ArduinoWiFi/1.1\r\n" +
                   "Connection: close\r\n\r\n");
  } else {
    Serial.println("connection failed");
    return;
  }
  String line;
  int repeatCounter = 0;
  while (!client.available() && repeatCounter < 10) {
    delay(500);
    Serial.println("w.");
    repeatCounter++;
  }

  while (client.connected() && client.available()) {
    char c = client.read();
    if (c == '[' || c == ']') c = ' ';
    line += c;
  }
  //Serial.println(line);
  client.stop();

  DynamicJsonBuffer jsonBuf;
  JsonObject &root = jsonBuf.parseObject(line);
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }

  temp = root["main"]["temp"];
  humidity = root["main"]["humidity"];
  pressure = root["main"]["pressure"];
  tempMin = root["main"]["temp_min"];
  tempMax = root["main"]["temp_max"];
  windSpeed = root["wind"]["speed"];
  clouds = root["clouds"]["all"];
}

