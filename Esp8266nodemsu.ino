#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <SPI.h>

// WiFi credentials
const char* ssid = "siam";
const char* password = "11111111";

// ThingSpeak settings
unsigned long myChannelNumber = 3074207; // Replace with your channel number
const char* myWriteAPIKey = "UF4ZVRAZWVVYE881"; // Replace with your API key

WiFiClient client;

// Variables
float temperature = 0, humidity = 0;
int gasValue = 0, soilResistiveRaw = 0, soilCapacitiveRaw = 0, co2Value = 0;
float soilResistivePercent = 0, soilCapacitivePercent = 0;

unsigned long lastUpdateTime = 0;
const unsigned long updateInterval = 20000; // 20 seconds

void setup() {
  Serial.begin(115200);
  Serial.println("ESP8266 Started");

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\n Wi-Fi connected!");

  // Initialize ThingSpeak
  ThingSpeak.begin(client);
}

void loop() {
  // Auto-reconnect WiFi if dropped
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    delay(5000);
    return;
  }

  if (Serial.available()) {
    String sensorData = Serial.readStringUntil('\n');
    sensorData.trim();

    // Expected format: temp,hum,gas,resistive,capacitive,co2
    int comma1 = sensorData.indexOf(',');
    int comma2 = sensorData.indexOf(',', comma1 + 1);
    int comma3 = sensorData.indexOf(',', comma2 + 1);
    int comma4 = sensorData.indexOf(',', comma3 + 1);
    int comma5 = sensorData.indexOf(',', comma4 + 1);

    if (comma1 > 0 && comma2 > comma1 && comma3 > comma2 && comma4 > comma3 && comma5 > comma4) {
      temperature = sensorData.substring(0, comma1).toFloat();
      humidity = sensorData.substring(comma1 + 1, comma2).toFloat();
      gasValue = sensorData.substring(comma2 + 1, comma3).toInt();
      soilResistiveRaw = sensorData.substring(comma3 + 1, comma4).toInt();
      soilCapacitiveRaw = sensorData.substring(comma4 + 1, comma5).toInt();
      co2Value = sensorData.substring(comma5 + 1).toInt();

      // Convert to percentages
      soilResistivePercent = map(soilResistiveRaw, 800, 300, 0, 100); // Adjust calibration
      soilCapacitivePercent = map(soilCapacitiveRaw, 300, 800, 0, 100); // Adjust calibration

      // Clamp values 0–100
      soilResistivePercent = constrain(soilResistivePercent, 0, 100);
      soilCapacitivePercent = constrain(soilCapacitivePercent, 0, 100);

      // Debug print
      Serial.println("\n Received Data:");
      Serial.print(" Temp: "); Serial.println(temperature);
      Serial.print(" Humidity: "); Serial.println(humidity);
      Serial.print(" Gas Value: "); Serial.println(gasValue);
      Serial.print(" Soil (Resistive): "); Serial.print(soilResistiveRaw);
      Serial.print(" -> "); Serial.print(soilResistivePercent); Serial.println("%");
      Serial.print(" Soil (Capacitive): "); Serial.print(soilCapacitiveRaw);
      Serial.print(" -> "); Serial.print(soilCapacitivePercent); Serial.println("%");
      Serial.print("CO₂ Value: "); Serial.println(co2Value);

      unsigned long currentTime = millis();
      if (currentTime - lastUpdateTime >= updateInterval) {
        lastUpdateTime = currentTime;

        // Send to ThingSpeak
        ThingSpeak.setField(1, temperature);
        ThingSpeak.setField(2, humidity);
        ThingSpeak.setField(3, gasValue);
        ThingSpeak.setField(4, soilResistivePercent);
        ThingSpeak.setField(5, soilCapacitivePercent);
        ThingSpeak.setField(6, co2Value);

        int responseCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
        if (responseCode == 200) {
          Serial.println("✅ Data sent to ThingSpeak successfully!");
        } else {
          Serial.print("❌ ThingSpeak update failed. HTTP error code: ");
          Serial.println(responseCode);
        }
      }
    } else {
      Serial.println("⚠️ Error: Malformed sensor data!");
    }
  }
}
