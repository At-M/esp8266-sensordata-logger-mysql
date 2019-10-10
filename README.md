# esp8266-sensordata-logger-mysql
This is a simple version of a BME280 / BMP280 Temperature-, Humidity-, Pressuredata logger to an external MySQL Database.

It is using no delays but millis() instead.

The sketch just uses a BME280 / BMP280 via I2C to measure the temperature, pressure and humidity and log it to an external MySQL database.

I haven't tested this on non-ESP8266 boards, nor have I tested it on other boards than the WeMos D1 Mini;

But I'm happy if you want to give me feedback nonetheless.


# Requirements:

## Hardware:

ESP8266 Compatible Board (Tested only with a WeMos D1 Mini!)

Bosch BME280 / BMP280 Sensor

**Pinout:**

**Sensor** | **WeMos**
-- | --
VIN | 3V3
GND | G
SCL | D1
SDA | D2

## Libraries:

#1 (most likely standard-included Libs now)
> Wire.h

> SPI.h (unsure if needed)

> ESP8266Wifi.h

> WiFiClient.h

#2 [MySQL Connector](https://github.com/ChuckBell/MySQL_Connector_Arduino)

#3 [Adafruits Sensor Library](https://github.com/adafruit/Adafruit_Sensor)

#4 [BME280I2C](https://github.com/finitespace/BME280)

## Database

Of course MySQL, might work with others too, check the responsible Library though.

My Database looks like this:

_(Using varchar for temp and humidity is a workaround for something else though)_

**Databasename:** autohome

**table**: temp


**Name of Field** | **Type** | **Null** | **Standard** | **Extra**
 --- | --- | --- | --- | ---
 id | int(11) | no | / | AUTO_INCREMENT
 sensorid | double | yes | NULL | /
 temp | varchar(25) | yes | NULL | /
 humidity | varchar(25) | yes | NULL | /
 date | timestamp | no | CURRENT_TIMESTAMP | /
 pressure | double | yes | NULL | /