# oven thermometer with MQTT #

Display temperature on screen and send temperate data to Node-RED UI through MQTT using RBP 3b+ as broker

![schematic]()

## parts ##
  - 2.8in SPI TFT ILI9341 screen
  - MAX6675 temperature sensor 
  - k-type thermocouple
  - NODEMCU ESP8266
  - RBP 3b+

## software ## 
- [Node-RED](https://github.com/node-red/node-red) 
    - [installation script](https://github.com/node-red/linux-installers) 
- [Mosquitto](https://mosquitto.org/man/mosquitto-8.html)
- PlatformIO

## sources ##
  - https://github.com/pawl/oven_thermometer
  - https://simple-circuit.com/esp8266-nodemcu-ili9341-tft-display/
  - https://randomnerdtutorials.com/esp32-mqtt-publish-subscribe-arduino-ide/