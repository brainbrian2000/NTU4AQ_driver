/*
  1. LASS Air Box
    a. 取得PMS5003T的資料：PM2.5、PM10.0、PM1.0
    b. 取得BME 280的資料：溫度、濕度、壓力
  2. 上傳ThingSpeak網站
*/
#include <Wire.h>
#include <SPI.h>

/*
     << PMS5003T 設定 >>
*/
#include <SoftwareSerial.h>
SoftwareSerial myMerial(3, 2);

long pmat10 = 0;
long pmat25 = 0;
long pmat100 = 0;
long Temp = 0;
long Humid = 0;
char buf[50];


/*
     << BME280 設定 >>
*/
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI
float BME_Temperature, BME_Pressure, BME_Humidity, BME_Altitude;


/*
     << 螢幕 設定 >>
*/
#include <Arduino.h>
#include <U8g2lib.h>


//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ 16, /* data=*/ 17);   // ESP32 Thing, HW I2C with pin remapping
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);


/*
     << WiFi 設定 >>
*/
#include <LWiFi.h>
char ssid[] = "IPCS_G202";      //  your network SSID (name)
char pass[] = "0233665082";  // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;               // your network key Index number (needed only for WEP)

String API_key = "RMHFNH48QOS55GVJ";
String ThingSpeakServer = "GET /update?key=";

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(117,185,24,248);
char server[] = "download.labs.mediatek.com";   // http://download.labs.mediatek.com/linkit_7697_ascii.txt

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;



void setup() {
  Serial.begin(9600);
  myMerial.begin(9600);


  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 10, "Wait...");
  u8g2.sendBuffer();

  // default settings
  unsigned status = bme.begin(0x76);
  // You can also pass in a Wire library object like &Wire2
  // status = bme.begin(0x76, &Wire2)

  Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(), HEX);
  Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
  Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
  Serial.print("        ID of 0x60 represents a BME 280.\n");
  Serial.print("        ID of 0x61 represents a BME 680.\n");
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
  }
  Serial.println("Connected to wifi");
  printWifiStatus();


  /*
       << 連上LinkIt 7697 測試用網頁 >>
  */
  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected to server (GET)");
    // Make a HTTP request:
    client.println("GET /linkit_7697_ascii.txt HTTP/1.0");
    client.println("Host: download.labs.mediatek.com");
    client.println("Accept: */*");
    client.println("Connection: close");
    client.println();
    delay(10);
  }
}

void loop() {
  // 更新Sensor資料
  retrievepm25();
  printBMEValues();

  // 顯示在螢幕上
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 10, "LinkIt AirBox");
  u8g2.drawStr(0, 25, ("PM2.5 : " + String(pmat25) + " ug/m3").c_str());
  u8g2.drawStr(0, 37, ("PM10 : " + String(pmat100) + " ug/m3").c_str());
  // u8g2.drawStr(0, 49, ("Temp : " + String(Temp) + " *C").c_str());
  // u8g2.drawStr(0, 61, ("Humid : " + String(Humid) + " %RH").c_str());
  u8g2.sendBuffer();

  delay(1000);

  // 顯示在螢幕上
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 10, "LinkIt AirBox");
  u8g2.drawStr(0, 25, ("Altitude : " + String(BME_Altitude) + " ug/m3").c_str());
  u8g2.drawStr(0, 37, ("Pressure : " + String(BME_Pressure) + " ug/m3").c_str());
  u8g2.drawStr(0, 49, ("Temp : " + String(BME_Temperature) + " *C").c_str());
  u8g2.drawStr(0, 61, ("Humid : " + String(BME_Humidity) + " %RH").c_str());
  u8g2.sendBuffer();
  delay(1000);

  // 建立WiFi連線
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
  }
  
  // 發送資料
  updateThingSpeak();

  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();
  }
}

void updateThingSpeak()
{
  String GET_String = ThingSpeakServer + API_key +
                      "&field1=" + String(pmat100) +
                      "&field2=" + String(pmat25) +
                      "&field3=" + String(pmat10) +
                      "&field4=" + String(BME_Temperature) +
                      "&field5=" + String(BME_Pressure) +
                      "&field6=" + String(BME_Humidity) +
                      "&field7=" + String(BME_Altitude);

  if (client.connect("api.thingspeak.com", 80)) {
    Serial.println("connected to server (GET)");
    Serial.println(GET_String);
    // Make a HTTP request:
    client.println(GET_String);
    client.println("Host: api.thingspeak.com");
    client.println("Accept: */*");
    client.println("Connection: close");
    client.println();
    delay(10);

  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void retrievepm25()
{
  int count = 0;
  unsigned char c;
  unsigned char high;

  while (myMerial.available())
  {
    c = myMerial.read();
    if ((count == 0 && c != 0x42) || (count == 1 && c != 0x4d)) {
      break;
    }
    if (count > 27) {
      break;
    }
    else if (count == 10 || count == 12 || count == 14 || count == 24 || count == 26) {
      high = c;
    }
    else if (count == 11) {
      pmat10 = 256 * high + c;
    }
    else if (count == 13) {
      pmat25 = 256 * high + c;
    }
    else if (count == 15) {
      pmat100 = 256 * high + c;
    }
    else if (count == 25) {
      Temp = (256 * high + c) / 10;
    }
    else if (count == 27) {
      Humid = (256 * high + c) / 10;
    }
    count++;
  }
  while (myMerial.available()) myMerial.read();
}

void printBMEValues() {
  BME_Temperature = bme.readTemperature();
  BME_Pressure = bme.readPressure() / 100.0F;
  BME_Humidity = bme.readHumidity();
  BME_Altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);

  Serial.print("Temperature = ");
  Serial.print(BME_Temperature);
  Serial.println(" *C");

  Serial.print("Pressure = ");
  Serial.print(BME_Pressure);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(BME_Altitude);
  Serial.println(" m");

  Serial.print("Humidity = ");
  Serial.print(BME_Humidity);
  Serial.println(" %");
  Serial.println();
}
