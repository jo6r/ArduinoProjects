/***
 * Temperature_Humidity_Sensor_v3
 * for NodeMcu ESP8266 with sensor DHT 21 (AM2301)
 * load data from sensor and post it to REST api
 * use deep sleep mode (RST pin to GPIO 16 which is labeled as D0)
*/
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Arduino_JSON.h>

// set up sensor DHT 21 (AM2301)
#define DHTTYPE DHT21       //sensor DHT 21 (AM2301)
uint8_t DHTPin = D4;        //use D4 pin
DHT dht(DHTPin, DHTTYPE);   //Initialize DHT sensor

// Time to sleep (in seconds):
const int sleepTimeS = 600; //600 = 10 minutes

const String api_url = "http://192.168.1.100:8080/sensorapi/v1/sensor-provider";
//const String api_url = "http://192.168.1.223:5000/sensorapi/v1/sensor-provider"; //ntb dev

//set up sensor name
const String sensorName = "room";
//set up location
const String location = "room";
//set up situated
const String situated = "indoor";

// set up wifi
const String ssid = "Moe";
const String password = "Klara30042016";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  dht.begin();  // init sensor

  delay(500); //Delay 0.5 sec. wait for sensor init is done

  //load temperature
  float temperature = dht.readTemperature();
  if (isnan(temperature)) {
    Serial.println(F("Failed to read temperature from DHT sensor!"));
  }

  //load humidity
  float humidity = dht.readHumidity();
  if (isnan(humidity)) {
    Serial.println(F("Failed to read humidity from DHT sensor!"));
  }

  //show values in console
  Serial.println("LocationName: " + sensorName);
  Serial.println("Temperature: " + String(temperature, 2) + " °C");
  Serial.println("Humidity: " + String(humidity, 2) + " %");
  Serial.println("");

  // connection to WIFI
  Serial.print("Connecting to "); 
  Serial.println(ssid); 
  WiFi.begin(ssid, password); 
  
  //check if ESP is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  
  Serial.println(WiFi.localIP());
  
  //prepare json for REST service
  JSONVar jsonObject;
  jsonObject["sensorname"] = sensorName;
  jsonObject["location"] = location;
  jsonObject["situated"] = situated;
  jsonObject["temperature"] = String(temperature, 2);
  jsonObject["humidity"] = String(humidity, 2);

  String jsonString = JSON.stringify(jsonObject);
  Serial.print("JSON.stringify(jsonObject): ");
  Serial.println(jsonString);

  //call REST service
  HTTPClient http;
  http.begin(api_url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Charset", "UTF-8");
  int httpCode = http.POST(jsonString); //Send the request
  String payload = http.getString(); //Get the response payload
  Serial.print("Response code: ");
  Serial.println(httpCode); //Print HTTP return code
  Serial.print("Response: ");
  Serial.println(payload); //Print request response payload
  http.end();
  
  WiFi.disconnect();
  Serial.println("WiFi disconnected..!");
  Serial.println("");

  //sleep
  Serial.println("in sleep mode");
  ESP.deepSleep(sleepTimeS * 1000000);
}

void loop() {
  // put your main code here, to run repeatedly:
}
