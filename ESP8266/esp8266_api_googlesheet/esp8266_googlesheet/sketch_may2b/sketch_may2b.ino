#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "Ravi's A16";
const char* password = "Ravi@0025";

// Replace with your Google Apps Script Web App URL
const char* serverName = "https://script.google.com/macros/s/AKfycbxyP7edWsCWG1-Cu9zjD9Vv6dZcjOx8fces8Pb-s3DX1d6yGoQ2gmekNUBssyU4AJ13dg/exec";

WiFiClientSecure client;

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");

  client.setInsecure(); // bypass SSL cert check
}

void loop() {
  delay(10000);

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/json");

    String postData = "{\"temperature\":" + String(t) + ",\"humidity\":" + String(h) + "}";
    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Response: " + response);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }
}
