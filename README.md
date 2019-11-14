Simple home automation using MQTT

Hardwared used:

- ESP32 board
- BME280 temperature and humidity sensor
- SDD1306 128x64 Oled display
- 433Mhz transmitter
- Relay board 5vts

Software: 

Some examples of home automation using MQTT protocol which is light enough to use in ESP32 boards. And as it's an standard, simple clients can be developed to monitor the data.

1) getting temperature and hmidity with BME280 and SD1306 display

sensing temperature and humidity using BME280 sensors and SDD1306 128x64 Oled displays to show the temperature in the device. 
The client will subscribe to a MQTT message like "home/temphumid/request/\*. It will publish only if the payload is "home/temphumid/request/all" or "home/temphumid/request/<client id>"
  
But if no explicit message is sent, the client will publish data each certain period of time, set in config and it will update display accordingly.


2) Controlling plugs with 433 Mhz emitter

turning on/off plugs responding to 433Mhz messages. We can find in the market plugs that are turned on and off with a remote control. The first step will be to "snif" the remote control to know which are the codes to send to the plug to turn it on and off. Then, the client will subscribe to MQTT message like, for instance, "house/plug/\*". Depending on the MQTT message payload, it will turn on or off three different plugs. The payload "house/plug/all" with value "on" will turn the three plugs on and with value "off" with turn all of them off.

3) Using a relay to turn on/off a circuit

Another simple example to open or close a circuit. In this case, we simulate an electronic pushbutton by closing circuit during 3 seconds. Same logic than in former examples, client subscribe to message. No publish from this client.

All the sources have a config.h file with the only few variables to set to configure wifi and mqtt broker:

```
============================================================ 
/* wifi settings */ 
const char* ssid     = <wifi ssid>;
const char* password = <wifi password>;

/* broker settings  */
/* broker hostname */
const char* broker_host = <mybrokerhost.example.com>;
/* broker port */
const int broker_port   = <broker port, i.e. 1883>;

/* broker user and password */
const char* broker_pwd =  <broker password>;
const char* broker_usr = <broker user>;
============================================================ 
```
