RFID Smart Card Top-Up System (ESP8266 + MQTT + Web Dashboard)
This project is a real-time IoT RFID wallet system that allows administrators to scan RFID cards, view balances live, and remotely top-up cards through a web dashboard. The system integrates ESP8266 hardware, an MQTT broker, a Node.js backend, and a real-time web interface.

LIVE WEB DASHBOARD
http://157.173.101.159:8215

PUBLIC GITHUB REPOSITORY
https://github.com/RUKUNDO7/Embedded

The repository contains all project components:
1. ESP8266 firmware
2. Backend API
3. Web Dashboard

SYSTEM ARCHITECTURE
Web Dashboard ⇄ Node.js Server ⇄ MQTT Broker ⇄ ESP8266 + RFID

HOW THE SYSTEM WORKS
RFID card is scanned by the/ESP8266. ESP8266 publishes card UID and balance via MQTT. Node.js server receives the message and pushes it to the dashboard using WebSockets. Admin enters a top-up amount on the dashboard. Backend publishes a top-up command via MQTT. ESP8266 updates the balance and sends the new balance back. Dashboard updates instantly in real time.

TECHNOLOGIES USED
1. Hardware
ESP8266 (NodeMCU)
MFRC522 RFID Reader

2. Backend
Node.js
Express.js
MQTT (Publish/Subscribe communication)
WebSockets (real-time updates)

3. Frontend
HTML
CSS
JavaScript

4. MQTT BROKER
157.173.101.159:1883

MQTT TOPICS
Card scanned → rfid/benise07/card/status
Send top-up → rfid/benise07/card/topup
Updated balance → rfid/benise07/card/balance

PROJECT STRUCTURE
project-root
esp8266/rfid_code/rfid_code.ino
backend/server.js
fronted/index.html
README.md
.gitignore

RUNNING THE BACKEND
1. Install dependencies:
npm install express mqtt ws cors body-parser
2. Start the server:
node server.js
Server runs at:
http://localhost:3000

UPLOADING ESP8266 FIRMWARE
1. Open rfid_code.ino in Arduino IDE
2. Install libraries:
ESP8266WiFi
PubSubClient
MFRC522
ArduinoJson
3. Update WiFi credentials
Upload the code to ESP8266

FEATURES
1. RFID card scanning
2. Real-time balance updates
3. Remote card top-up
4. MQTT IoT communication
5. Live dashboard using WebSockets

AUTHOR
Gihozo Rukundo Benise
Rwanda Coding Academy – IoT Project
