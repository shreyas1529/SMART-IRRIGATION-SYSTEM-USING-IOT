```markdown
# 🌱 Smart Irrigation System Using IoT

## 📌 Project Overview
This project automates irrigation in **nurseries and botanical gardens** using **IoT**. It monitors **soil moisture, temperature, humidity, and weather forecasts** to optimize water usage and reduce wastage.

## 🛠 Features
✔️ **Automatic & Manual Irrigation Mode**  
✔️ **Real-time Sensor Monitoring**  
✔️ **Weather Forecast Integration (OpenWeather API)**  
✔️ **ESP8266-Based Web Control Panel**  
✔️ **Water Conservation & Smart Decision Making**  

## 🏗 System Architecture
- **Microcontroller**: ESP8266
- **Sensors**: Soil Moisture, DHT11 (Temperature & Humidity)
- **Relay Module**: Controls water pump
- **Connectivity**: Wi-Fi (for remote monitoring)
- **Cloud API**: OpenWeather API for weather forecasting

## ⚙️ Circuit Diagram
![Circuit Diagram](Docs/Circuit_Diagram.png)

## 🔧 Installation & Setup
### 1️⃣ **Hardware Setup**
- Connect **ESP8266, sensors, relay module**, and **water pump** as per the circuit diagram.

### 2️⃣ **Software Requirements**
- Install **Arduino IDE** & **ESP8266 Board Package**
- Required Libraries:
  ```cpp
  #include <ESP8266WiFi.h>
  #include <DHT.h>
  #include <ArduinoJson.h>
  ```
- Configure **WiFi & API Key** in `config.h` file.

### 3️⃣ **Upload Code to ESP8266**
- Open `irrigation_system.ino` in Arduino IDE.
- Upload code to ESP8266.
- Open Serial Monitor (`115200 baud`) to debug.

### 4️⃣ **Access Web Interface**
- Open browser and enter:
  ```
  http://<ESP8266_IP>
  ```

## 📊 Web Dashboard
- **Monitor:** Soil Moisture, Temperature, Humidity, Weather Forecast
- **Control:** Toggle Irrigation (Manual/Auto)

![Web UI](Images/Web_UI_Screenshot.png)

## 🚀 Future Enhancements
🔹 AI-based predictive irrigation  
🔹 Mobile app integration  
🔹 Solar-powered system  

---
👨‍💻 Developed by: **Shreya S. & Vedha H.V** | **Mentors: Madhusudhan N, Chetan Naik J**  
🎓 **Ramaiah Skill Academy | Embedded System Design**
```
