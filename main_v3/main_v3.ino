#include <NTPClient.h>

#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
 
#include "DHT.h"
#define DHTPIN 14     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
 
#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

#define RXp2 16
#define TXp2 17
 
float humidity;
float temperature;
float oxygen;
float volumetricWaterContent;
bool alarm_mail = false;
String inputString = "";

// for timestamp
String timestamp;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7200, 60000);
// -------------------------------------------------------
 
DHT dht(DHTPIN, DHTTYPE);
 
WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);
 
void connectAWS()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 
  Serial.println("Connecting to Wi-Fi");
 
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
 
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);
 
  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);
 
  // Create a message handler
  client.setCallback(messageHandler);
 
  Serial.println("Connecting to AWS IOT");
 
  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(100);
  }
 
  if (!client.connected())
  {
    Serial.println("AWS IoT Timeout!");
    return;
  }
 
  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
 
  Serial.println("AWS IoT Connected!");
}
 
void publishMessage()
{
  StaticJsonDocument<200> doc;
  doc["timestamp"] = timestamp;
  doc["humidity"] = humidity;
  doc["temperature"] = temperature;
  doc["oxygen"] = oxygen;
  doc["Volumetric Water Content"] = volumetricWaterContent;
  if (alarm_mail)
  {
    doc["alarm_mail"] = true;
  }
  else
  {
    doc["alarm_mail"] = false;
  }
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
 
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}
 
void messageHandler(char* topic, byte* payload, unsigned int length)
{
  Serial.print("incoming: ");
  Serial.println(topic);
 
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char* message = doc["message"];
  Serial.println(message);
}

void updateVariables()
{
  alarm_mail = false;
  char start = ' ';
  char escape = '.';
  while(start == ' ' or start == '\n') start = Serial2.read();
  if (start == int(escape)){
      // show timestamp
      Serial.println(timestamp);
      int variable = Serial2.read() - '0';
      char inChar = (char)Serial2.read();
      while (inChar != '\n'){
        inputString += inChar;
        inChar = (char)Serial2.read();
      }
      switch(variable){
        case 0:
          temperature = inputString.toFloat();
          Serial.print("updating temperature in AWS: "); Serial.print(temperature);
          inputString = "";
          break;
        case 1:
          humidity = inputString.toFloat();
          Serial.print("updating humidity in AWS: "); Serial.print(humidity);
          inputString = "";
          break;
        case 2:
          volumetricWaterContent = inputString.toFloat();
          Serial.print("updating Volumetric Water Content in AWS: "); Serial.print(volumetricWaterContent);
          inputString = "";
          break;
        case 3:
          oxygen = inputString.toFloat();
          Serial.print("updating oxygen in AWS: "); Serial.print(oxygen);
          inputString = "";
          break;
        case 4:
          if (inputString.toInt()){
            alarm_mail = true;
          }
          Serial.print("send alarm: "); Serial.print(alarm_mail);
          inputString = "";
          break;
        default:
          inputString = "";
          break;
      }
    }
}

String getFormattedDate(time_t epochTime) {
  char buffer[20];
  char date[20];
  strftime(buffer, 20, "%d/%m/%Y", localtime(&epochTime));
  strftime(date, 20, "%d%m%Y", localtime(&epochTime));
  timestamp = String(date);
  return String(buffer);
}

void getTime()
{
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  Serial.println("Date: " + getFormattedDate(epochTime));
  Serial.println("Time: " + timeClient.getFormattedTime());
  timestamp +=  timeClient.getFormattedTime();
  delay(1000);
}
 
void setup()
{
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);
  inputString.reserve(200);
  connectAWS();
  // for timestamp
  timeClient.begin();
  dht.begin();
}
 
void loop()
{
    
    getTime();
    if (Serial2.available() > 0){
      Serial.println("Message Received: ");
      updateVariables();
      publishMessage();
      Serial.println(Serial2.readString());
      client.loop();
      delay(1000);
    }
}
