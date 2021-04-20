/*
    oven thermometer with MQTT

    based on:
      - https://github.com/pawl/oven_thermometer
      - https://simple-circuit.com/esp8266-nodemcu-ili9341-tft-display/
      - https://randomnerdtutorials.com/esp32-mqtt-publish-subscribe-arduino-ide/

    parts:
      - 2.8in SPI TFT ILI9341 screen
      - MAX6675 temperature sensor 
      - k-type thermocouple
      - NODEMCU ESP8266
      - RBP 3b+

    software: 
      - Node-RED - Low-code programming for event-driven applications 
        - https://github.com/node-red/node-red
        - installation script: https://github.com/node-red/linux-installers
      - Mosquitto - MQTT broker 
        - https://mosquitto.org/man/mosquitto-8.html
      - Platformio - IDE

*/

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_I2CDevice.h>
#include <max6675.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

// wifi credentials
const char *ssid =     "network_name";
const char *password = "network_password";

// MQTT
WiFiClient espClient;
PubSubClient client(espClient);

const char *mqtt_server = "raspberry_pi_ip";  // IP to broker (raspberry pi)

// screen pinout
#define TFT_CS  D2
#define TFT_RST D3
#define TFT_DC  D4

// screen constructor
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// MAX6675 pinout
#define thermoSCK D8
#define thermoSO  D1
#define thermoCS  D0

// MAX6675 constructor
MAX6675 thermocouple(thermoSCK, thermoCS, thermoSO);

// globals
unsigned long lastMsg = 0;
float temperature = 0;

// get messages from broker
void callback(char *topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(" . Message: ");

  String messageTemp;

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if (String(topic) == "esp8266/output") {
    
    Serial.print("Changing output to ");

    if (messageTemp == "on") {
      Serial.println("on");
      tft.println("Slider Status: 1");
    }
    else if (messageTemp == "off") {
      Serial.println("off");
      tft.println("Slider Status: 0");
    }
  }
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");

    // Attempt to connect
    if (client.connect("ESP8266Client"))
    {
      Serial.println("connected");
      client.subscribe("esp8266/output");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(9600);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  tft.begin();

  tft.setRotation(3);             // set screen to landscape
  tft.fillScreen(ILI9341_BLACK);  // set background to black

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop()
{
  tft.setTextWrap(false);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_GREEN, ILI9341_RED);           // green text with red background
  tft.setTextSize(6);

  tft.printf("%.2f F\n", thermocouple.readFahrenheit());  // print temperature to screen

  tft.setTextSize(2);
  tft.print("Local IP: ");
  tft.println(WiFi.localIP());

  if (!client.connected()) {
    reconnect();
  }
  client.loop();  // check for messages from client

  unsigned long now = millis();
  if (now - lastMsg > 1000) {     // transmit message every second
    lastMsg = now;

    temperature = thermocouple.readFahrenheit();
    char tempString[8];
    dtostrf(temperature, 1, 2, tempString);
    client.publish("esp8266/temperature", tempString);
  }

  delay(1000);
}