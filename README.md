# ESP32C6-Zigbee2MQTT
An **ESP32C6 based zigbee coordinator** to bridge Zigbee devices to MQTT (similar in idea to Zigbee2MQTT). 
Works with an ESP32 + CC2530 Zigbee module to control smart bulbs, sensors, switches, etc. via MQTT. 
## features
- Zigbee coordinator on **ESP32C6**
- Integrates with **MQTT** brokers
- Supports motion sensors, temperature sensors, smart bulbs, smart sockets
- Web UI for joining and controlling devices

Everything is done via a webinterface. Below is an example of the frontpage, showing joined devices.<br>
<img width="642" height="300" alt="frontpag" src="https://github.com/user-attachments/assets/fdf6abbf-44c2-4497-9801-9567ac84e8fe" />

# downloads
2026-10-16 download [ESP32C6-Zigbee2Mqtt_v0_2](https://github.com/patience4711/ESP32C6-Zigbee2MQTT/blob/main/ESP32C6_ZIGBEE_0_2.ino.bin)<br>

## what can it do
- easy to connect to your wifi network using the captive portal.<br>
- setup, join and control devices like smart lamps, smart sockets, sensors
- everything is done via webinterface
- devices can be controlled via mosquitto and webui
- data is sent via mosquitto
- we can make a device voice controlled via domotica software (homey, etc).

Below an example of the webui to control a smart bulb. This works very good.<br>
<img width="764" height="606" alt="rgb_ui" src="https://github.com/user-attachments/assets/6d8a6adc-392e-46a8-b8c1-826a382d8638" />

## global working
The coordinator starts a zigbee network. Within the network, the coordinator can read and send messages to the devices, provided that they are joined. The incoming messages are state and readings data, eg temperature of a sensor.
### devices
We can add a device to the system via the webui. Once added, we can join it. This means that the device is known by the coordinator and has an address.
### Control a device
In the frontpage we get a list of all devices. When it is a switch or a bulb, there is a link to a control page. In case of a smart bulb we can put it on/off, set color temperature, a color by slide the hue and staturation and dim the bulb.
### read measures
In case of a sensor device, it sends messages when there is motion detected, or temp/hum readings.
These messages are analized and associated with a certain device and a adequate mosquitto message is sent.
