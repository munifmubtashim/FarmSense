#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>

#define espSerial Serial1 // Use Serial1 for ESP8266

// Constants
#define DHTPIN 7        // Pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT22 sensor
#define MQ135_PIN A8    // MQ135 air quality sensor
#define SOIL_RESISTIVE A3  // Resistive soil moisture sensor
#define SOIL_CAPACITIVE A6 // Capacitive soil moisture sensor

DHT dht(DHTPIN, DHTTYPE); // Initialize DHT22 sensor

// Variables
float temperature = 0, humidity = 0;
int gasValue = 0;
int soilResistiveValue = 0;
int soilCapacitiveValue = 0;
int co2Value = 0;

unsigned long lastSensorReadTime = 0;
const unsigned long sensorInterval = 2000; // 2 seconds

void setup() {
  Serial.begin(9600);
  espSerial.begin(115200);
  dht.begin();

  Serial.println(" Smart Farming Sensor System Started");
  Serial.println("---------------------------------------");
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastSensorReadTime >= sensorInterval) {
    lastSensorReadTime = currentTime;

    // MQ135 Reading
    int mqRaw = analogRead(MQ135_PIN);
    gasValue = map(mqRaw, 0, 1023, 0, 500);        // 0-500 scale
    co2Value = map(mqRaw, 0, 1023, 400, 2000);     // CO2 ppm estimate

    // Soil Moisture Readings
    soilResistiveValue = analogRead(SOIL_RESISTIVE);
    soilCapacitiveValue = analogRead(SOIL_CAPACITIVE);

    // DHT22 Temperature & Humidity
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();

    // Send data via ESP8266 as CSV
    if (!isnan(temperature) && !isnan(humidity)) {
      espSerial.print(temperature); espSerial.print(",");
      espSerial.print(humidity); espSerial.print(",");
      espSerial.print(gasValue); espSerial.print(",");
      espSerial.print(soilResistiveValue); espSerial.print(",");
      espSerial.print(soilCapacitiveValue); espSerial.print(",");
      espSerial.println(co2Value);
    }

    // ---------------- Debug Output ----------------
    Serial.println("\n Sensor Report");
    Serial.println("---------------------------------------");
    Serial.print("Temp: "); Serial.print(temperature); Serial.println(" °C");
    Serial.print("Humidity: "); Serial.print(humidity); Serial.println(" %");
    Serial.print("Gas Sensor: "); Serial.print(gasValue); Serial.println(" (0–500)");
    Serial.print("CO₂ Estimate: "); Serial.print(co2Value); Serial.println(" ppm");
    Serial.print("Soil Resistive: "); Serial.println(soilResistiveValue);
    Serial.print("Soil Capacitive: "); Serial.println(soilCapacitiveValue);

    // Temperature Conditions
    if (!isnan(temperature)) {
      if (temperature >= 10 && temperature < 18) {
        Serial.println("❄️ Cool  Suitable for cold-weather crops");
      } else if (temperature >= 18 && temperature <= 25) {
        Serial.println(" Temperature Normal Optimal range");
      } else if (temperature > 25 && temperature <= 32) {
        Serial.println("☀️ Warm Monitor closely");
      } else {
        Serial.println("⚠️ Temperature out of defined range!");
      }
    }

    // Air Quality Conditions
    if (gasValue <= 100) {
      Serial.println(" Air Quality Excellent");
    } else if (gasValue <= 200) {
      Serial.println("Air Quality Good");
    } else if (gasValue <= 300) {
      Serial.println(" Air Quality Moderate Monitor Needed");
    } else {
      Serial.println(" Poor Air Quality  Consider Ventilation");
    }

    // CO2 Levels
    if (co2Value < 800) {
      Serial.println("CO₂ Level Normal");
    } else if (co2Value <= 1500) {
      Serial.println(" CO₂ Slightly Elevated – Monitor Room");
    } else if (co2Value <= 3000) {
      Serial.println(" High CO₂ – Ventilate Now");
    } else {
      Serial.println("Dangerous CO₂ – Immediate Action Needed!");
    }

    // Soil Moisture Resistive
    if (soilResistiveValue > 600) {
      Serial.println(" Soil (resistive) TOO DRY – Water Needed");
    } else if (soilResistiveValue > 400) {
      Serial.println(" Soil (resistive) Moisture Normal");
    } else {
      Serial.println(" Soil (resistive) TOO WET – Stop Watering");
    }

    // Soil Moisture Capacitive
    if (soilCapacitiveValue < 300) {
      Serial.println(" Soil (capacitive) TOO DRY – Water Needed");
    } else if (soilCapacitiveValue <= 700) {
      Serial.println("Soil (capacitive) Moisture Normal");
    } else {
      Serial.println(" Soil (capacitive) TOO WET – Stop Watering");
    }

    Serial.println("---------------------------------------");
  }
}
