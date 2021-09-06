#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp; // I2C

#define SEALEVELPRESSURE_HPA (1013.25)

//#define BMP_SCK  (22)
//#define BMP_MISO (21)
//#define BMP_MOSI (23)
//#define BMP_CS   (5)
const int AirValue = 3320;
const int WaterValue = 1520;

int soilMoistureValue = 0;
int soilmoisturepercent = 0;

float temperature, pressure, altitude;

const char* ssid = "add here your network ssid";
const char* password = "type here your wifi password";

IPAddress local_IP(192, 168, 0, 133);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

WebServer server(9090);

void setup() {
  Serial.begin(9600);
  delay(100);

  //  bmp.begin(0x76);
  if (!bmp.begin(0x76)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);

  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to local wi-fi network
  WiFi.config (local_IP, gateway, subnet);
  WiFi.begin(ssid, password);

  //check wifi is connected to the wifi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

  server.on("/metrics", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
}


void loop() {
  server.handleClient();

  soilMoistureValue = analogRead(A4);
  soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
  Serial.println(soilMoistureValue);
  Serial.println(soilmoisturepercent);
  delay(1000);

}

void handle_OnConnect() {
  temperature = bmp.readTemperature();
  //  humidity = bmp.readHumidity();
  pressure = bmp.readPressure();
  //  altitude = bmp.readAltitude(SEALEVELPRESSURE_HPA);
  altitude = bmp.readAltitude(1013.25);
  server.send(200, "text/html", SendHTML(temperature, pressure, altitude));
  //  server.send(200, "text/html", SendHTML(temperature,pressure));
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

String SendHTML(float temperature, float pressure, float altitude) {
  String ptr = "\n";
  ptr += "Temperature ";
  ptr += temperature;
  ptr += "\n";
  ptr += "Atmospheric_Pressure ";
  ptr += pressure;
  ptr += "\n";
  ptr += "Altitude ";
  ptr += altitude;
  ptr += "\n";
  ptr += "Soil_Moisture_Value ";
  ptr += soilMoistureValue;
  ptr += "\n";
  ptr += "Soil_Moisture_Percentage ";
  ptr += soilmoisturepercent;
  ptr += "\n";
  return ptr;
}
