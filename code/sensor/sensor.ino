/* Sensor Interface v 1.0
    Made by David S. - At-M - in 2018
    Line 19 - 34 is changeable
*/
// Generally needed
#include <MySQL_Connection.h> // MySQL Connector
#include <MySQL_Cursor.h> // MySQL Connector
#include <ESP8266WiFi.h> // Wifi Connector

//BME/BMP280 specific
#include <Wire.h> // Wire Library
#include <Adafruit_Sensor.h> // Adafruit Sensor Library
#include <BME280I2C.h> // include BME280 Sensor (0x76)

BME280I2C bme;    // Default : forced mode, standby time = 1000 ms Oversampling = pressure ×1, temperature ×1, humidity ×1, filter off,

// Changeable Stuff
int sensorname = 4; // Sensor no. to differentiate results
long interval = 900000; // Interval at which to check data in ms (15min = 900000ms, 30sek = 30000ms) DONT CHECK FASTER THAN 2000ms! The BME/BMP will heat up or behave strangely!

char ssid[] = "XXX"; // Your Networkname
char pass[] = "XXX"; // Your Networkpassword

IPAddress ip(0, 0, 0, 0); // Ip of this device in your network
IPAddress gateway(0, 0, 0, 0); // Gateway this device looks for (The same IP as your router should work in most cases)
IPAddress subnet(255, 255, 255, 0); // Subnet this device looks for (255.255.255.0 should work in most cases)

IPAddress server_addr(0, 0, 0, 0); // IP of your MySQL Server
char user[] = "XXX"; // Your MySQL user
char password[] = "XXX"; // Your MySQL password

char INSERT_SQL[] = "INSERT INTO DATABASE.TABLE(sensorid, temp, humidity, pressure) VALUES (%i, %s, %s, %s)"; // replace this with the query you want/need

// Do not change stuff below this line if you're not knowing what you're doing!
int wificonnect = 0; // checks the wificonnection 1 = connected
int connectloop = 0; // counts the number of tries

WiFiServer server(80);
WiFiClient client;
MySQL_Connection conn((Client *)&client);
char query[128]; // max query length

long previousMillis = 0; // Will store the last time when sensors were read


int dbconn = 0;
float var_temp(NAN), var_hum(NAN), var_pres(NAN);

// Functionprototypes
void checksensor();
void wififunction();
void dbupload(char*, char*, char*);

// Runs once
void setup() {
  Serial.begin(115200);
  Wire.begin();
  //WiFi.persistent(false); // to fix unnecessary writing to flash and "random" connections to wifi, does not seem to work properly
  Serial.print("Sensor: ");
  Serial.println(sensorname);
  Serial.print(F("IP : "));
  Serial.println(ip);
  Serial.println("");
  Serial.println("AT+CWMODE_CUR=1<CR>"); // Sets ESP8266 as Client (avoids broadcasting a network)
  Serial.println("");
}
// Repeating code
void loop() {
  int i = 0;
  unsigned long currentMillis = millis();
  // run following code after each interval (ms)
  if (currentMillis - previousMillis > interval) {
    // saves the time at which it last measured
    previousMillis = currentMillis;


    do {
      checksensor(); // Tests which Sensor is currently connected

      // Sets the specific units, use Celsius or Fahrenheit here.
      // TODO: Move this into the "changeable lines"
      BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
      BME280::PresUnit presUnit(BME280::PresUnit_Pa);
      // reads sensordata
      bme.read(var_pres, var_temp, var_hum, tempUnit, presUnit);
      // Converts the input into useable variables
      char var_Ctemp[10];
      char var_Chum[10];
      char var_Cpres[20];
      dtostrf(var_temp, 1, 1, var_Ctemp);
      dtostrf(var_hum, 1, 1, var_Chum);
      dtostrf(var_pres, 1, 1, var_Cpres);
      // If the sensor functioned as it should
      if ((!isnan(var_hum)) && (!isnan(var_temp))) {
        Serial.println("Sensor has been read successfully");
        // TODO: Use the Pressuremeasurement to calculate the height etc.
        wififunction();
        // Sucks if you review your code after some time and didn't add comments..
      }
      connectloop = 0; // reset the variable for the next time this runs (used in wififunction())
      dbconnect();
      dbupload(var_Ctemp, var_Chum, var_Cpres);

      i++;
    } while ((!isnan(var_hum)) && (!isnan(var_temp)) && (i < 1));
    i = 0; // reset the loop for the next time this runs
    // close db connection
    dbconn = 0;// reset the variable for the next time this runs (used in dbconnect())
    conn.close();
    Serial.println("Closed db connection");
    // disconnect wifi to save power
    wificonnect = 0; // reset the variable for the next time this runs (used in wififunction())
    WiFi.disconnect();
    Serial.println("Closed wifi connection");
    Serial.println("Waiting for the next loop...");
    Serial.println("");
    // TODO: use deepsleep
  }
  delay(20); // Workaround for an internal ESP8266 issue
}
void checksensor() {
  Serial.println("Searching for sensors...");
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
}
// Connect to WiFi
void wififunction() {
  do {
    Serial.println("###WiFi NOT Connected##");
    WiFi.config(ip, gateway, subnet);
    Serial.println("Initialising connection");
    Serial.print(F("Setting static ip to : "));
    Serial.println(ip);
    Serial.println("");
    WiFi.mode(WIFI_STA); // Set WiFi to Clientmode
    WiFi.begin(ssid, pass);
    Serial.print("Connecting to ");
    Serial.println(ssid);
    wificonnect = 1;
    connectloop++;

  } while ((wificonnect != 1) && (connectloop < 6)); // Aborts after 5 times to save power
  Serial.println("Wifi has been connected successfully");
  Serial.println("");
  Serial.print("Assigned Hostname: ");
  Serial.println(WiFi.hostname());
  Serial.println("");
  Serial.print("Assigned IP: ");
  Serial.println(WiFi.localIP());
}
// Connect to Database
void dbconnect() {
  int j = 0;
  do {
    Serial.println("Connecting to Database...");
    conn.connect(server_addr, 3306, user, password); // 3306 = standard port
    dbconn = 1;
    j++;
  } while ((dbconn != 1) && (j < 6));
  j = 0; // reset the loop for the next time this runs
  Serial.println("Db has been connected successfully");
}
void dbupload(char* var_Ctemp, char* var_Chum, char* var_Cpres) {
  // Upload the data
  // print query into the serial monitor for easy debugging
  sprintf(query, INSERT_SQL, sensorname, var_Ctemp, var_Chum, var_Cpres);

  Serial.println("Recording data..");
  Serial.println(query);
  // run query
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  cur_mem->execute(query);
  delete cur_mem;
}

