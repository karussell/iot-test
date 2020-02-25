# What's this?

A playground to record temperature or detect weather changes with homegrown software.

![image](https://user-images.githubusercontent.com/129644/64167917-b325a000-ce4a-11e9-8ffc-fabad79eb2c8.png)

## Hardware

Buy ESP 32 dev kit, e.g. from [here](https://www.az-delivery.de/products/esp32-developmentboard?ls=de). 
Add some sensors like DHT22, a breadboard and a [mini display](https://www.az-delivery.de/products/0-96zolldisplay?ls=de).

## Software

### ESP 32

 * install [vscode](https://code.visualstudio.com/) (open source and runs on all major platforms)
 * install platformIO for vscode under "View"->"Extensions"
 * then inside the platformIO plugin install the "espressif 32" platform (under "platforms")
 * install clang compiler for your operating system

For linux you additionally need to create some special stuff in `/etc/udev/rules.d/99-platformio-udev.rules` and then do `sudo chmod 666 /dev/ttyUSB0`. Internet should help you further if you have problems here.

Compile and upload the main.cpp to your ESP32. Maybe start with the Serial communication, add the display and then the sensor. 

The display pins should be connected via:

| ESP | OLED|
|-----|-----|
| GND | GND |
| 3V3 | VCC |
| G21 | SDA |
| G22 | SCL |

The sensor DHT sensor is expected to be connected with pin G27 (DHT library).

The DS sensor is expected to be connected with pin G26 (using the OneWire and DallasTemperature library).

### WLAN and server communication working

Change the WLAN credentials in code including the host where your node.js web app runs, e.g. localhost:8000. See the web subfolder for more details.

### Server

Install node JS and type `node web/app.js`
