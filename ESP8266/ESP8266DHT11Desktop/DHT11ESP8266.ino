#include "DHT.h"

// For NodeMCU: Connect DHT11 DATA to pin D4 (GPIO 2)
#define DHTPIN 2     
#define DHTTYPE DHT11   

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Try 74880 first; if not available in Monitor, use 115200
  Serial.begin(115200); 
  delay(3000); // CRITICAL: Wait for boot garbage to clear
  
  Serial.println("\n--- Communication Restored ---");
  dht.begin();
}

void loop() {
  delay(2000); 

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Reading failed! Verify DATA is in pin D4.");
    return;
  }

  Serial.print("Humidity: "); Serial.print(h);
  Serial.print("%  |  Temperature: "); Serial.print(t);
  Serial.println("°C");
}
