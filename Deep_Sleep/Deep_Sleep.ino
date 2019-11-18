/***
 * Deep Sleep for ESP8266
 * RST pin to GPIO 16 which is labeled as D0
 * https://randomnerdtutorials.com/esp8266-deep-sleep-with-arduino-ide/
 * https://openhomeautomation.net/esp8266-battery
*/

const int sleepTimeS = 10;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.setTimeout(2000);
  
  // Wait for serial to initialize.
  while(!Serial) {
    Serial.print(".");
  }

  Serial.println("ESP8266 in normal mode");
  
  delay(5000);
  
   // Sleep
  Serial.println("ESP8266 in sleep mode");
  ESP.deepSleep(sleepTimeS * 1000000);
}

void loop() {
  // put your main code here, to run repeatedly:
}
