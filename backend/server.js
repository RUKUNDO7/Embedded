const express = require("express");
const mqtt = require("mqtt");
const WebSocket = require("ws");
const cors = require("cors");

const app = express();
app.use(cors());
app.use(express.json());

const team_id = "benise07";
const mqttClient = mqtt.connect("mqtt://157.173.101.159");

const server = app.listen(3000, () => {
  console.log("Server running on http://localhost:3000");
});

const wss = new WebSocket.Server({ server });

mqttClient.on("connect", () => {
  console.log("Connected to MQTT broker");
  const topics = [
    `rfid/${team_id}/card/status`,
    `rfid/${team_id}/card/balance`
  ];

  mqttClient.subscribe(topics, (err) => {
    if (!err) {
      console.log("Subscribed to card status and balance topics");
    } else {
      console.error("MQTT subscribe error:", err);
    }
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
  let payload = message.toString();

  try {
    const parsed = JSON.parse(payload);
    if (typeof parsed.balance === "undefined" && typeof parsed.new_balance !== "undefined") {
      parsed.balance = parsed.new_balance;
    }
    payload = JSON.stringify(parsed);
  } catch (err) {
    console.warn("Non-JSON MQTT payload forwarded as-is");
  }

  broadcast(payload);
});
