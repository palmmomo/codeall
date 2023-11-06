#define MQ_7 34
#if defined(ESP32)
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"
#elif defined(ESP8266)
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#define DEVICE "ESP8266"
#endif

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

const char* ssid = "TTTA@DOM";
const char* password = "Ttta@2021";

AsyncWebServer server(80);

#define WIFI_SSID "TTTA@DOM"
#define WIFI_PASSWORD "Ttta@2021"
#define INFLUXDB_URL "https://influx.thetigerteamacademy.net"
#define INFLUXDB_TOKEN "eMcuZw7jmVPBgxBLMYRiJAcxu38AXHnSSZhRpEOxrKE1i_AjZkYuC9w0xMqv-EP8bLA6CyM-zKZj-4dDLEo51Q=="
#define INFLUXDB_ORG "TTTA"
#define INFLUXDB_BUCKET "TTTA ENERGY"
#define TZ_INFO "UTC +7"


InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
Point sensor("CO_MEASUREMENT");

float analysis(int);
float Rs;

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  analogReadResolution(10);

  analogSetAttenuation(ADC_0db);

  Serial.print("Connecting to wifi");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", "/update for update code");
  });

  AsyncElegantOTA.begin(&server);
  server.begin();
  Serial.println("HTTP server started");

  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}

void loop() {
  sensor.clearFields();
  int sensorValue = analogRead(MQ_7);
  float coConcentration = analysis(sensorValue);
  // Serial.print("adc : ");
  // Serial.print(sensorValue);
  // Serial.print("\t");
  // Serial.print("Carbon monoxide : ");
  // Serial.print(analysis(sensorValue), 3);
  // Serial.print("ppm");
  // Serial.print("\t");
  // Serial.print("Sensor resistance : ");
  // Serial.println(Rs);
  // delay(100);

 sensor.addField("CO_float", coConcentration);

  Serial.print("Writing: ");
  Serial.println(sensor.toLineProtocol());

  if ((WiFi.RSSI() == 0) && (wifiMulti.run() != WL_CONNECTED)) {
    Serial.println("Wifi connection lost");
  }

  if (!client.writePoint(sensor)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }

  delay(100);
}

float analysis(int adc) {
  float slope = -0.7516072988;
  float A = 45.87510694;
  float Rseries = 1000;
  float V_Rseries = ((float)adc * 3.3) / 4095;
  Rs = ((3.3 - V_Rseries) / V_Rseries) * Rseries;
  float R0 = 400;
  float Y = Rs / R0;
  float CO_ppm = pow(10, (log10(Y / A) / slope));
  return CO_ppm;
}
