#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const int mq7Pin = A0; // MQ-7 connected to analog pin A0

const char* ssid = "Ravi's A16";
const char* password = "Ravi@0025";

// Replace with your Google Apps Script Web App URL
const char* serverName = "https://script.google.com/macros/s/AKfycbzBFT-q9BsW6xVZhYNzZKLhlf1MUPp_EQZJa2bN2AaUwFv5W_gnwslr2OPyiQpIEsziIg/exec";

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
  int mq7Value = analogRead(mq7Pin); // Raw ADC value (0–1023)
  float mq7Voltage = mq7Value * (1.0 / 1023.0); // Convert to voltage (0–1V range)

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Print sensor values before sending
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" °C | Humidity: ");
  Serial.print(h);
  Serial.print(" % | MQ7 Raw: ");
  Serial.print(mq7Value);
  Serial.print(" | MQ7 Voltage: ");
  Serial.println(mq7Voltage);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/json");

    // Include MQ-7 data in JSON payload
    String postData = "{\"temperature\":" + String(t) +
                      ",\"humidity\":" + String(h) +
                      ",\"mq7_raw\":" + String(mq7Value) +
                      ",\"mq7_voltage\":" + String(mq7Voltage) + "}";

    // Debug: print payload before sending
    Serial.println("Payload being sent: " + postData);

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
