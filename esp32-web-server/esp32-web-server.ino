/*
 * ESP32 Web Server
 *
 * Libraries:
 * WiFi                    2.0.0   C:\Users\<name>\AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.17\libraries\WiFi
 * ESPAsyncWebServer       3.1.0   C:\Users\<name>\Documents\Arduino\libraries\ESPAsyncWebServer
 * FS                      2.0.0   C:\Users\<name>\AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.17\libraries\FS
 * AsyncTCP                1.1.4   C:\Users\<name>\Documents\Arduino\libraries\AsyncTCP
 * DHT sensor library      1.4.6   C:\Users\<name>\Documents\Arduino\libraries\DHT_sensor_library
 * Adafruit Unified Sensor 1.1.14  C:\Users\<name>\Documents\Arduino\libraries\Adafruit_Unified_Sensor
 *
 * Board Packages Used:
 * Arduino ESP32 Boards    2.0.13  NodeMCU-32S
 * ESP32 by Espressif      2.0.17  ..Arduino15\packages\esp32\hardware\esp32\2.0.17\...
 *
 * Links:
 * - Implementing Web Server on ESP32
 *   https://projecthub.arduino.cc/cetech11/implementing-web-server-on-esp32-5c24be
 * - ESP32 Web Server
 *   https://esp32io.com/tutorials/esp32-web-server
 * - ESP32 with DHT11/DHT22 Temperature and Humidity Sensor using Arduino IDE
 *   https://randomnerdtutorials.com/esp32-dht11-dht22-temperature-humidity-sensor-arduino-ide/
 * - ESP32 DHT11/DHT22 Web Server – Temperature and Humidity using Arduino IDE
 *   https://randomnerdtutorials.com/esp32-dht11-dht22-temperature-humidity-web-server-arduino-ide/
 *
 */

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DHT.h>
#include "defines.h"
#include "config.h"

const char* ssid = SSID;
const char* password = PASS;
float humidity = 0.0;
float temperature = 0.0;

// initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

AsyncWebServer server(80);

void setup() {
  // sensor data pin needs a pullup
  pinMode(DHTPIN, INPUT_PULLUP);

  // starts serial interface
  Serial.begin(115200);

  // connect to wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
	delay(1000);
	Serial.println("Connecting to wifi...");
  }
  Serial.println("Connected to wifi");

  // Print the ESP32's IP address
  Serial.print("ESP32 web server's IP address: ");
  Serial.println(WiFi.localIP());

  // define a route to serve the HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
	Serial.println("ESP32 web server: request received:");
	Serial.println("GET /");
	request->send_P(200, "text/html", R"rawliteral(<html><body>
		<h1>Hello, ESP32!</h1>
		<p>
			<h2>Temperature</h2>
			<span>%TEMPERATURE%</span>
		</p>
		<p>
			<h2>Humidity</h2>
			<span>%HUMIDITY%</span>
		</p>
	</body></html>)rawliteral", sensorValue);
  });

  // start the server
  server.begin();

  // start sensor
  dht.begin();
}

void loop() {
  // wait between measurements (slow sensor)
  delay(2000);

  // temperature or humidity read takes about 250ms!
  float h = dht.readHumidity();

  // read temperature as Celsius (default)
  float t = dht.readTemperature();

  // check if any read failed and exit early without update
  if (isnan(h) || isnan(t)) {
    Serial.println(F("DHT sensor read failed!"));
    return;
  }

  // update humidity and temperature values
  humidity = h;
  temperature = t;

  // compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(temperature, humidity, false);

  Serial.print(F("H: "));
  Serial.print(humidity);
  Serial.print(F("%  T: "));
  Serial.print(temperature);
  Serial.print(F("C  (heat index: "));
  Serial.print(hic);
  Serial.println(F("C)"));
}

String sensorValue(const String& var){
  if(var == "TEMPERATURE"){
    return String(temperature);
  }
  else if(var == "HUMIDITY"){
    return String(humidity);
  }
  return String();
}