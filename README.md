# ESP32-Zigbee2MQTT
An **ESP32 based zigbee coordinator** to bridge Zigbee devices to MQTT (similar in idea to Zigbee2MQTT). 
Works with an ESP32 + CC2530 Zigbee module to control smart bulbs, sensors, switches, etc. via MQTT. 
## features
- Zigbee coordinator on **ESP32**
- Integrates with **MQTT** brokers
- Supports motion sensors, temperature sensors, smart bulbs, smart sockets
- Web UI for joining and controlling devices

Everything is done via a webinterface. Below is an example of the frontpage, showing joined devices.<br>
![front_6_1](https://github.com/user-attachments/assets/e92021f0-b7fd-49e9-bea4-a19ee2662a60)


# downloads
firmware for [the cc2530](https://github.com/patience4711/ESP32-Zigbee2MQTT/blob/main/CC2530ZNP-Prod.bin)<br>
firmware for [the cc2531](https://github.com/patience4711/ESP32-Zigbee2MQTT/blob/main/CC2531ZNP-Prod.bin)<br>
2026-01-06 download [ESP32-Zigbee2Mqtt_v1_4](https://github.com/patience4711/ESP32-Zigbee2MQTT/blob/main/ESP32_ZIGBEE_1_4.ino.bin)<br>

## simple hardware: 
 - An ESP32<br>
 - A CC2530 zigbeemodule<br>
 - A usb power supply<br>

## what can it do
- easy to connect to your wifi network using the captive portal.<br>
- setup, join and control devices like smart lamps, smart sockets, sensors
- everything is done via webinterface
- devices can be controlled via mosquitto
- data is sent via mosquitto
- it can work with domoticz so we can make it speech controlled via google home.

Below an example of the webui to control a smart bulb. This works very good.<br>
![ESP32-Zigbee2Mqtt-bulbs](https://github.com/user-attachments/assets/83acdabb-9065-4b9d-ba8a-c8fb06770a4d)

## global working
The coordinator starts a zigbee network. Within the network, the coordinator can read and send messages to the devices, provided that they are joined. The incoming messages are state and readings data, eg temperature of a sensor.
### devices
We can add a device to the system via the webui. Once added, we can join it. This means that the device is known by the coordinator and has an address.
### Control a device
In the frontpage we get a list of all devices. When it is a switch or a bulb, there is a link to a control page. In case of a smart bulb we can put it on/off, set color temperature, a color by slide the hue and staturation and dim the bulb.
### read measures
In case of a sensor device, it sends messages when there is motion detected, or temp/hum readings.
These messages are analized and associated with a certain device and a adequate mosquitto message is sent.
