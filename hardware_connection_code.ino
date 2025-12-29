/***********************************************************************
  NodeMCU (ESP8266) + Adafruit IO + LED(any hardware device)

  Flow:
  Razorpay → Webhook → Pipedream → Adafruit IO feed "amount"
          → NodeMCU subscribes to "amount" → LED ON(machine turns on) when correct payment received
 ***********************************************************************/

//*********************** Libraries
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

//*********************** Pins
// Use D1 for external LED (through 220Ω resistor to GND),
// or change to LED_BUILTIN if you want to use the onboard LED.
#define LED_PIN D1

//*********************** WiFi Credentials  (CHANGE THESE)
#define WLAN_SSID "wifi username"
#define WLAN_PASS "wifi password"

//*********************** Adafruit IO Credentials (CHANGE THESE)
#define AIO_SERVER     "io.adafruit.com"
#define AIO_SERVERPORT 1883          // 1883 = MQTT (non-SSL)
#define AIO_USERNAME   "adafruit username"
#define AIO_KEY        "adafruit activation key"
#define FeedName       "amount"      // Your feed key on Adafruit IO

/************ Global State ************/

WiFiClient client;

// MQTT client
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT,
                          AIO_USERNAME, AIO_KEY);

// Subscribe object for the "amount" feed
Adafruit_MQTT_Subscribe amountFeed =
  Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/" FeedName);

/*************************** Setup ************************************/

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);  // LED off initially

  Serial.println();
  Serial.println(F("Starting NodeMCU Payment LED Example"));
  Serial.print(F("Connecting to WiFi: "));
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);

  // Wait for WiFi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println(F("WiFi connected!"));
  Serial.print(F("IP address: "));
  Serial.println(WiFi.localIP());

  // Subscribe to Adafruit IO feed
  mqtt.subscribe(&amountFeed);
}

/*************************** Main Loop ********************************/

void loop() {
  // Make sure we’re connected to MQTT
  MQTT_connect();

  Adafruit_MQTT_Subscribe *subscription;

  // Wait (up to 5 seconds) for a new MQTT message
  while ((subscription = mqtt.readSubscription(5000))) {

    if (subscription == &amountFeed) {
      Serial.print(F("Received amount: "));
      Serial.println((char *)amountFeed.lastread);

      // Convert string to integer
      int payment = atoi((char *)amountFeed.lastread);

      // If payment > 0, treat as successful payment
      if (payment > 0) {
        Serial.println(F("Payment detected! Turning LED ON."));
        digitalWrite(LED_PIN, HIGH);   // LED ON
        delay(10000);                  // keep ON for 10 seconds
        digitalWrite(LED_PIN, LOW);    // LED OFF
        Serial.println(F("LED OFF, waiting for next payment..."));
      } else {
        Serial.println(F("Payment value <= 0, ignoring."));
      }
    }
  }
}

/********************* MQTT Connect Helper ****************************/

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print(F("Connecting to Adafruit IO MQTT... "));

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // 0 = success
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println(F("Retrying MQTT connection in 5 seconds..."));
    mqtt.disconnect();
    delay(5000);
    retries--;
    if (retries == 0) {
      // Give up and let watchdog reset
      while (1) {
        delay(1);
      }
    }
  }
  Serial.println(F("MQTT Connected!"));
}
