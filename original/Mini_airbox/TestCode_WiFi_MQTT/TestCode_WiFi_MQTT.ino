/*
  這個範例是用wifi 連上網路後
  以MQTT發送資料 (PubSubClient ver2.7.0
  API 參考：https://pubsubclient.knolleary.net/api.html
*/
#include <LWiFi.h>
#include <String.h>
#include <PubSubClient.h>

char ssid[] = "GAIA";     //  your network SSID (name)
char pass[] = "72956799";         // your network password
int status = WL_IDLE_STATUS;      // the Wifi radio's status

WiFiClient client;
PubSubClient upload(client);


#define SERVER_IP "test.mosquitto.org"
#define SERVER_PORT 1883
#define USERNAME ""
#define PASSWORD ""
#define CLIENT_ID "GAIA_RD_72956799"
#define TOPIC "NTU/Arduino/MQTT/test"


void reconnect()
{
  // Loop until we're reconnected
  while (!upload.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (upload.connect(CLIENT_ID, USERNAME, PASSWORD))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(upload.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  upload.subscribe(TOPIC);  // 只要連線後就訂閱Topic
//  upload.unsubscribe(TOPIC);  // 取消訂閱Topic
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();

  upload.setServer(SERVER_IP, SERVER_PORT);
  upload.setCallback(callback);

  delay(1500);

}

void loop() {
  if (!upload.connected())
  {
    reconnect();
  }
  else
  {
    // get a random number from 1 to 100
    long randNumber = random(100) + 1;
    String payload = "{\"ID\":"+String(CLIENT_ID)+",\"value\":" + String(randNumber) + "}";
    // publish to QIoT Server
    /*
      if (upload.publish(TOPIC, payload.c_str()))
      {
      Serial.println("Message => " + payload + " has been sent to " + SERVER_IP + ".");
      }
      delay(100);
   */
    delay(1000);
  }

  upload.loop();

}

void printWifiData() {
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);

}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  Serial.print(bssid[5], HEX);
  Serial.print(":");
  Serial.print(bssid[4], HEX);
  Serial.print(":");
  Serial.print(bssid[3], HEX);
  Serial.print(":");
  Serial.print(bssid[2], HEX);
  Serial.print(":");
  Serial.print(bssid[1], HEX);
  Serial.print(":");
  Serial.println(bssid[0], HEX);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}
