# FarmSense
An IoT-based smart greenhouse framework to enhance food security and support climate action in Bangladesh.

## 📌 Overview  
FarmSense is an IoT-based smart greenhouse system designed to monitor and control environmental parameters such as temperature, humidity, CO₂ concentration, and soil moisture. The framework aims to improve food security and support climate action by enabling efficient resource management in agriculture.  

## ⚙️ Features  
- 🌡️ Real-time monitoring of **temperature and humidity**  
- 💨 **CO₂ concentration estimation** using MQ-135 sensor  
- 💧 Soil moisture detection with **resistive & capacitive sensors**  
- 📡 Wireless data transmission via **ESP8266**  
- 📊 Calibration equations embedded for **accurate sensor readings**  
- 🔔 Threshold-based alerts for pollution and soil moisture  

## 📂 Repository Structure  
```
├── Arduinocode.ino            # Arduino sketch (main greenhouse logic)
├── ESP8266nodemsu.ino         # ESP8266 code for IoT connectivity
├── micro_project              # Project report (PDF/DOCX)
└── README.md                  # You are here
```
## 🧮 Calibration & Data Acquisition  
- **Temperature (°C):** `T = T_raw`  
- **Humidity (%):** `H = H_raw`  
- **CO₂ (ppm):** `CO2 = 3.2 * (G - 300) + 400`  
- **Soil Moisture (Resistive):** `Moisture_R = (1 - (V - 300)/500) * 100`  
- **Soil Moisture (Capacitive):** `Moisture_C = (1 - (V - 400)/400) * 100`  

All computations are embedded in the ESP8266 firmware for **real-time monitoring**.  

## 🚀 Getting Started  
1. Clone the repository:  
   ```bash
   git clone https://github.com/your-username/FarmSense.git
