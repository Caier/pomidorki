# Pomidorki

An arduino project made for ESP8266 based boards. Its intended use is to automatically monitor the humidity of house plants and water them if necessary. All the data is also presented on a web interface where you can see the humidity level, water tank level, and water the plants manually. You can also make it send warnings to a Discord webhook when the plants run dry or the water tank level is low.

Used components:
- Wemos D1 mini ESP8266 board
- Resistive humidity sensors
- 5V water pumps
- Ultrasonic proximity sensor (to measure the water tank level)