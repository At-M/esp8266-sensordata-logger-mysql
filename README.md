# esp8266-sensordata-logger-mysql
This is a simple version of a BME280 / BMP280 Temperature-, Humidity-, Pressuredata logger to an external MySQL Database.

It is using no delays but millis() instead. (The so called state machine)

The sketch just uses a BME280 / BMP280 via I2C to measure the temperature, pressure and humidity and log it to an external MySQL database.

I haven't tested this on non-ESP8266 board, nor have I tested it on other boards than the WeMos D1 Mini;
But I'm happy if you want to give me feedback nonetheless.


Requirements:

Hardware:

#1 An ESP8266 Compatible Board
  > Tested only with a WeMos D1 Mini!
#2 Bosch BME280 / BMP280 Sensor

Libraries:

#1 (most likely standard-included Libs now)
Wire.h
SPI.h (unsure if needed)
ESP8266Wifi.h
WiFiClient.h

#2 MySQL Connector
https://github.com/ChuckBell/MySQL_Connector_Arduino

#3 Adafruits Sensor Library
https://github.com/adafruit/Adafruit_Sensor

#4 BME280I2C
https://github.com/finitespace/BME280
