/***
 * Temperature_Humidity_Sensor_v2
 * for NodeMcu ESP8266 with sensor DHT 21 (AM2301)
 * load data from sensor and post it to REST api
*/

#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Arduino_JSON.h>

// set up sensor DHT 21 (AM2301)
#define DHTTYPE DHT21       //sensor DHT 21 (AM2301)
uint8_t DHTPin = D4;        //use D4 pin
DHT dht(DHTPin, DHTTYPE);   //Initialize DHT sensor

//set up location name
const String sensorName = "kitchen";

// set up wifi
const String ssid = "ssid";
const String password = "password";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
//  delay(10000); //Delay 10 sec.
  delay(600000); //Delay 10 min.

  // connection to WIFI
  Serial.print("Connecting to "); 
  Serial.println(ssid); 
  //connect to your local wi-fi network
  WiFi.begin(ssid, password); 
  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  
  Serial.println(WiFi.localIP());

  //load temperature
  float temperature = dht.readTemperature();
  if (isnan(temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
  }

  //load humidity
  float humidity = dht.readHumidity();
  if (isnan(humidity)) {
    Serial.println(F("Failed to read from DHT sensor!"));
  }

  //show values to console
  Serial.println("LocationName: " + sensorName);
  Serial.println("Humidity: " + String(temperature) + " %");
  Serial.println("Temp: " + String(humidity) + " °C");
  Serial.println("");

  //prepare json for REST service
  JSONVar jsonObject;
  jsonObject["SensorName"] = sensorName;
  jsonObject["Temperature"] = temperature;
  jsonObject["Humidity"] = humidity;

  String jsonString = JSON.stringify(jsonObject);
  Serial.print("JSON.stringify(jsonObject): ");
  Serial.println(jsonString);

  //call REST service
  HTTPClient http;
  http.begin("http://192.168.1.100:8888/v1/measurement"); //jarvis server
  //http.begin("http://192.168.1.139:5000/v1/measurement"); //ntb dev
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
}
