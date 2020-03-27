/***
 * Temperature_Humidity_Sensor_v4
 * for NodeMcu ESP8266 with sensor DHT 21 (AM2301)
 * load data from sensor and publish it to mqtt queue
 * use deep sleep mode (RST pin to GPIO 16 which is labeled as D0)
*/

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <DHT.h>

// Time to sleep (in seconds):
//const int sleepTimeS = 600; //600 = 10 minutes
const int sleepTimeS = 3600; //3600 = 1hour

//sensor settings
const String sensorName = "ChildrensRoom1";
const String mqtt_topic = "home/indoor/sensor/" + sensorName;

//mqtt
const char* mqttServer = "192.168.1.100";
const int mqttPort = 1883;
const char* mqttUser = "";
const char* mqttPassword = "";

//wifi
const String ssid = "Moe";
const String password = "password";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// set up sensor DHT 21 (AM2301)
#define DHTTYPE DHT21   //sensor DHT 21 (AM2301)
#define DHTPIN D4       // pin
DHT dht(DHTPIN, DHTTYPE);   //Initialize DHT sensor

void wifiConnect() {
  WiFi.begin(ssid, password);
  
//  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  
  Serial.print("Connected, IP address: ");
  Serial.print(WiFi.localIP());
  Serial.print(", SSID: ");
  Serial.println(ssid);
}

void wifiDisconnect() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFi.disconnect();
//    Serial.println("Disconnected from WIFI");
  }
}

void mqttConnect() {
  mqttClient.setServer(mqttServer, mqttPort);
  
  while (!mqttClient.connected()) {
//    Serial.println("Connecting to MQTT...");
  
    if (mqttClient.connect("Client", mqttUser, mqttPassword)) {
      Serial.println("Connected to MQTT");
    } else {
      Serial.print("MQTT failed with state ");
      Serial.print(mqttClient.state());
      delay(1000);
    }
  }
}

void mqttDisconnect() {
  if (mqttClient.connected()) {
    mqttClient.disconnect();
//    Serial.println("Disconnected from MQTT");
  }
}

float loadTemperature() {
  float temperature = dht.readTemperature();
  if (isnan(temperature)) {
    Serial.println(F("Failed to read temperature from DHT sensor!"));
  } 
  return temperature; 
}

float loadHumidity() {
  float humidity = dht.readHumidity();
  if (isnan(humidity)) {
    Serial.println(F("Failed to read humidity from DHT sensor!"));
  }
  return humidity; 
}

void setup() {
  Serial.begin(115200);
  dht.begin();  // init sensor
  delay(500); //Delay 0.5 sec. wait for sensor init is done
 
  const size_t capacity = JSON_OBJECT_SIZE(4);
  DynamicJsonDocument doc(capacity);
  doc["name"] = sensorName;
  doc["temperature"] = loadTemperature();
  doc["humidity"] = loadHumidity();

  char buffer[512];
  serializeJson(doc, buffer);
  
  wifiConnect();
  mqttConnect();

  //String to char array
  int str_len = mqtt_topic.length() + 1;
  char char_array[str_len];
  mqtt_topic.toCharArray(char_array, str_len); 
  
  mqttClient.publish(char_array, buffer);
  Serial.print("Message sent ");
  Serial.println(buffer);
  
  mqttDisconnect();
  wifiDisconnect();

  //sleep
  Serial.println("in sleep mode");
  ESP.deepSleep(sleepTimeS * 1000000);
}

void loop() {}
