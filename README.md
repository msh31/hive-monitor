# 🐝 Hive Monitor
Hardware side of an assignment from my course for a client.

#### This repository is only 1/3 of the project, you can find the other repos here:
- [Frontend](https://github.com/DaanBouma/hive-frontend)
- [Backend](https://github.com/punthenk/Bee_API)

---

## Before you build
Make a copy of the ``config.h.example`` file and enter your wifi credentials and the location of an endpoint.

## Building
### Requirements
- [ESP IDF framework](https://github.com/espressif/esp-idf)
- Basic knowledge of the ``C`` programming language
- An ESP32 to test your code
- (Optional) DHT11 sensor & HX711 sensor connected to tbe ESP32

### Instructions
1. Initialize the development environment
```bash
#Windows
Open the ESP IDF profile in Windows Terminal

# MacOS (and linux?) run this in your terminal:
'. $HOME/esp/esp-idf/export.sh'
```
2. Use the following commands to build and or flash
```bash
# build (make sure you are in the main/ folder)
idf.py build

# flash - {PORT} = COM{x} on Windows & /dev/cu.usbserial-xxxx on MacOS / Linux (? not sure)
idf.py -p {PORT} flash monitor
```
