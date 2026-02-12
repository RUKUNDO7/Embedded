const express = require("express");
const mqtt = require("mqtt");
const WebSocket = require("ws");
const bodyParser = require("body-parser");
const cors = require("cors");

const app = express();
app.use(cors());
app.use(bodyParser.json());

const team_id = "benise07";
const mqttClient = mqtt.connect("mqtt://157.173.101.159");

const server = app.listen(3000, () => {
  console.log("Server running on http://localhost:3000");
});

const wss = new WebSocket.Server({ server });

mqttClient.on("connect", () => {
  console.log("Connected to MQTT broker");
  mqttClient.subscribe(`rfid/${team_id}/card/status`, (err) => {
    if (!err) console.log("Subscribed to balance topic");
    else console.error("MQTT subscribe error:", err);
  });
});

mqttClient.on("error", (err) => console.error("MQTT error:", err));

function broadcast(message) {
  wss.clients.forEach(client => {
    if (client.readyState === WebSocket.OPEN) {
      client.send(message);
    }
  });
}

app.post("/topup", (req, res) => {
  const { uid, amount } = req.body;
  console.log("Topup request received:", { uid, amount });

  if (!uid || !amount || isNaN(amount) || amount <= 0) {
    console.warn("Invalid top-up request");
    return res.status(400).json({ message: "Invalid UID or amount" });
  }

  const payload = JSON.stringify({ uid, amount });
  mqttClient.publish(`rfid/${team_id}/card/topup`, payload, () => {
    console.log("Published top-up to MQTT:", payload);
  });

  res.json({ message: "Top-up sent!" });
});

mqttClient.on("message", (topic, message) => {
  console.log("MQTT message received:", topic, message.toString());
  broadcast(message.toString());
});
