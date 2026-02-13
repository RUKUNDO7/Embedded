#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ArduinoJson.h>

#define SS_PIN 4   // D2
#define RST_PIN 5  // D1

const char* ssid = "EdNet";
const char* password = "Huawei@123";
const char* mqtt_server = "157.173.101.159";

String team_id = "benise07";

WiFiClient espClient;
PubSubClient client(espClient);
MFRC522 mfrc522(SS_PIN, RST_PIN);

int balance = 3000;

void setup_wifi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    retries++;
    if (retries > 40) { // after 20 seconds, restart
      Serial.println("WiFi connection failed, restarting...");
      ESP.restart();
    }
  }

  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("]");

  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, payload, length);
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  int amount = doc["amount"];
  balance += amount;
  Serial.print("Top-up received. Amount: ");
  Serial.print(amount);
  Serial.print(", New balance: ");
  Serial.println(balance);

  StaticJsonDocument<200> response;
  response["uid"] = doc["uid"];
  response["new_balance"] = balance;

  char buffer[256];
  serializeJson(response, buffer);

  String topic_pub = "rfid/" + team_id + "/card/balance";
  client.publish(topic_pub.c_str(), buffer);
  Serial.println("Published updated balance to MQTT.");
}

void reconnect() {
  Serial.println("Attempting MQTT connection...");
  while (!client.connected()) {
    if (client.connect("ESP8266Client")) {
      Serial.println("MQTT connected");
      String topic_sub = "rfid/" + team_id + "/card/topup";
      client.subscribe(topic_sub.c_str());
      Serial.print("Subscribed to topic: ");
      Serial.println(topic_sub);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" trying again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(460800);
  delay(1000);
  Serial.println("ESP8266 starting...");

  SPI.begin();
  Serial.println("SPI started");

  mfrc522.PCD_Init();
  Serial.println("MFRC522 RFID initialized");

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  Serial.println("Setup complete!");
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  Serial.println("Card detected!");

  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uid += String(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.print("Card UID: ");
  Serial.println(uid);

  StaticJsonDocument<200> doc;
  doc["uid"] = uid;
  doc["balance"] = balance;

  char buffer[256];
  serializeJson(doc, buffer);

  String topic_pub = "rfid/" + team_id + "/card/status";
  client.publish(topic_pub.c_str(), buffer);
  Serial.println("Published card status to MQTT");

  delay(2000);
}
