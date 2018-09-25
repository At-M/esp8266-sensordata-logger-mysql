/* Sensor Interface v 1.0
    Made by David S. - At-M - in 2018
    Search for XXX to see changeable things
*/
// Generally needed
#include <MySQL_Connection.h> // MySQL Connector
#include <MySQL_Cursor.h> // MySQL Connector
#include <ESP8266WiFi.h> // Wifi Connector
#include <WiFiClient.h> // Wifi Connector

//BME/BMP280 specific
#include <Wire.h> // Wire Library (not sure if needed)
#include <SPI.h> // SPI Bus Library (not sure if needed)
#include <Adafruit_Sensor.h> // Adafruit Sensor Library
#include <BME280I2C.h> // include BME280 Sensor (0x76)

BME280I2C bme;    // Default : forced mode, standby time = 1000 ms Oversampling = pressure ×1, temperature ×1, humidity ×1, filter off,
int sensorname = 4; // Sensor no. to differentiate results, NOT USED YET (search for insert_sql to find the query)

char ssid[] = "XXX"; // Your Networkname
char pass[] = "XXX"; // Your Networkpassword
byte mac[6];
int wificonnect = 0;
int connectschleife = 0; // counts the tries

WiFiServer server(80);
IPAddress ip(0, 0, 0, 0); // Ip of this device in your network XXX
IPAddress gateway(0, 0, 0, 0); // Gateway this device looks for (The same IP as your router should work in most cases) XXX
IPAddress subnet(255, 255, 255, 0); // Subnet this device looks for (255.255.255.0 should work in most cases) XXX

WiFiClient client;
MySQL_Connection conn((Client *)&client);

char INSERT_SQL[] = "INSERT INTO DATABASE.TABLE(sensorid, temp, humidity, pressure) VALUES (4, %s, %s, %s)"; // replace this with the query you want/need, the 4 is to differentiate between different sensors XXX
char query[128];

IPAddress server_addr(0, 0, 0, 0); // IP of your MySQL Server XXX
char user[] = "XXX"; // Your MySQL user 
char password[] = "XXX"; // Your MySQL password 

long previousMillis = 0; // Will store last time sensors were read
long interval = 900000; // Interval at which to check data in ms (15min = 900000ms, 30sek = 30000ms) XXX

int i = 0;
int j = 0;
int dbconn = 0;
float var_temp(NAN), var_hum(NAN), var_pres(NAN);
void setup() {
  //runs once
  Serial.begin(115200);
  Wire.begin();
  //WiFi.persistent(false); // to fix unnecessary writing to flash and "random" connections to wifi, does not seem to work properly
}

void loop() {
  //repeating code here

  unsigned long currentMillis = millis();
  // run following after each interval (ms)
  if (currentMillis - previousMillis > interval) {
    // saves the time at which it last measured
    previousMillis = currentMillis;

    // read data
    do {
      //BME/BMP280
      while (!bme.begin())
      {
        Serial.println("Could not find BME280 sensor!");
        delay(1000);
      }
      switch (bme.chipModel())
      {
        case BME280::ChipModel_BME280:
          Serial.println("Found BME280 sensor! Success.");
          break;
        case BME280::ChipModel_BMP280:
          Serial.println("Found BMP280 sensor! No Humidity available.");
          break;
        default:
          Serial.println("Found UNKNOWN sensor! Error!");
      }
      // Sets the specific units, use Celsius or Fahrenheit here.
      BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
      BME280::PresUnit presUnit(BME280::PresUnit_Pa);
      bme.read(var_pres, var_temp, var_hum, tempUnit, presUnit);
      char var_Ctemp[10];
      char var_Chum[10];
      char var_Cpres[20];
      dtostrf(var_temp, 1, 1, var_Ctemp);
      dtostrf(var_hum, 1, 1, var_Chum);
      dtostrf(var_pres, 1, 1, var_Cpres);
      // If the sensor functioned as it should
      if ((!isnan(var_hum)) && (!isnan(var_temp))) {

        Serial.println("Sensor has been read successfully");
        // TO DO: Use the Pressuremeasurement to calculate the height etc.
        // Connect to wifi
        Serial.println("###WiFi NOT Connected##");
        WiFi.config(ip, gateway, subnet);
        Serial.println("Initialising connection");
        Serial.print(F("Setting static ip to : "));
        Serial.println(ip);
        Serial.println("");
        WiFi.begin(ssid, pass);
        Serial.print("Connecting to ");
        Serial.println(ssid);
        wificonnect = 1;
        connectschleife++;

      } while ((wificonnect != 1) && (connectschleife < 6)); // Aborts after 5 times to save power
      // WAIT DID I USE AN IF/WHILE HERE ? THAT EXISTS?
      // Sucks if you review your code after some time and didn't add comments..
      connectschleife = 0;
      do {
        Serial.println("Wifi has been connected successfully");
        Serial.print("MAC: ");  Serial.print(mac[5], HEX);  Serial.print(":");  Serial.print(mac[4], HEX);  Serial.print(":");  Serial.print(mac[3], HEX);  Serial.print(":");  Serial.print(mac[2], HEX);  Serial.print(":");  Serial.print(mac[1], HEX);  Serial.print(":");  Serial.println(mac[0], HEX);
        Serial.println("");
        Serial.print("Assigned IP: ");
        Serial.print(WiFi.localIP());
        // Connect to database
        conn.connect(server_addr, 3306, user, password); // 3306 = standard port
        dbconn = 1;
        j++;
      } while ((dbconn != 1) && (j < 6));
      j = 0;
      Serial.println("Db has been connected successfully");
      // Upload the data
      // print query into the serial monitor for easy debugging
      sprintf(query, INSERT_SQL, var_Ctemp, var_Chum, var_Cpres);

      Serial.println("Recording data..");
      Serial.println(query);
      // run query
      MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
      cur_mem->execute(query);
      delete cur_mem;

      i++;
    } while ((!isnan(var_hum)) && (!isnan(var_temp)) && (i < 1));
    i = 0;
    // close db connection
    dbconn = 0;
    conn.close();
    Serial.println("Closed db connection");
    // disconnect wifi to save power
    wificonnect = 0;
    WiFi.disconnect();
    Serial.println("Closed wifi connection");

    // TODO: use deepsleep

  }
  delay(20); // Workaround for an internal ESP8266 issue
}
