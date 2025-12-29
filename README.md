## Smart-Payment-Activated-Device-Control-Using-Cloud-Based-Webhooks-an
This project demonstrates a real-time IoT-based automation system where a device (machine) is activated automatically after a successful online payment of particular amount.

Components Used:
NodeMCU (ESP8266)
Adafruit IO
Pipedream (Webhook automation)
Razorpay (Payment gateway)
LED (Hardware automation)
Wi-Fi network

Flow  Diagram:
User Payment (Razorpay)
        ↓
     Webhook
        ↓
     Pipedream
        ↓
   Adafruit IO Feed
        ↓
     NodeMCU (MQTT)
        ↓
     Device ON
