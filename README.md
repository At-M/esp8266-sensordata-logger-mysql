# esp8266-sensordata-logger-mysql
This is a simple version of a BME280 / BMP280 Temperature-, Humidity-, Pressuredata logger to an external MySQL Database.

It is using no delays but millis() instead. (The so called state machine)

The sketch just uses a BME280 / BMP280 via I2C to measure the temperature, pressure and humidity and log it to an external MySQL database.

I haven't tested this on non-ESP8266 board, nor have I tested it on other boards than the WeMos D1 Mini;
But I'm happy if you want to give me feedback nonetheless.
