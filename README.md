This program is a sort of Zigbee2MQTT on an ESP32C6 platform. What it does is the following:
- it runs a zigbee network via a coordinator
- it can join any zigbee device.
- the joined devices report readings (temperature or motion sensor) that are send via mqtt
- via webui, devices can be controlled (rgb-bulb, smart socket, etc)
- via mqtt devices can be controlled
- via domotica systems (like Homey), devices can be voice controlled.

The frontpage shows the joined devices wit their readings and links to control pages.
<img width="642" height="300" alt="frontpag" src="https://github.com/user-attachments/assets/0cfd8b56-e89c-4d93-83ed-f3a8ea95f9c3" /><br>


  en example of an ui to control a led bulb
  <img width="764" height="606" alt="rgb_ui" src="https://github.com/user-attachments/assets/609767b0-4eb0-48c4-be84-8b4b3ab0fc4a" />
### download
