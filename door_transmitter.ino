/*
  Project Name:   doors
  Developer:      Eric Klein Jr. (temp2@ericklein.com)
  Description:    device that tracks door open/close status in cloud
  
  See README.md for target information, revision history, feature requests, etc.
*/

// Library initialization
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "Adafruit_IO_Client.h"

// function prototypes
void battery_level();
void door_open();

// Assign Arduino pins
#define DOOR 13

// Global variables
// battery level reporting interval in minutes
#define BATTERY_INTERVAL 60
// deep sleep interval in seconds
#define SLEEP_LENGTH 60
// wifi credentials
#define WLAN_SSID       "Klein AP"
#define WLAN_PASS       "KleinAPin09"
//#define WLAN_SSID       "Lemnos Guest"
//#define WLAN_PASS       "collhardware2017"
// Adafruit IO access key
#define AIO_KEY         "7bda6629571247bda1c3262740a93293"

// create an Adafruit IO client instance
WiFiClient client;
Adafruit_IO_Client aio = Adafruit_IO_Client(client, AIO_KEY);

void setup() {
  Serial.begin(115200);
  Serial.println("HUZZAH Trigger Basic");
  
  EEPROM.begin(512);
  pinMode(DOOR, INPUT_PULLUP);

  // get the current count position from eeprom
  byte battery_count = EEPROM.read(0);

  // we only need this to happen once every X minutes,
  // so we use eeprom to track the count between resets.
  if(battery_count >= ((BATTERY_INTERVAL * 60) / SLEEP_LENGTH)) {
    // reset counter
    battery_count = 0;
    // report battery level to Adafruit IO
    battery_level();
  } else {
    // increment counter
    battery_count++;
  }

  // save the current count
  EEPROM.write(0, battery_count);
  EEPROM.commit();

  // if door isn't open, we don't need to send anything
  if(digitalRead(DOOR) == LOW) {
    Serial.println("Door closed");
    // we don't do anything
  } else {
    // the door is open if we have reached here,
    // so we should send a value to Adafruit IO.
    Serial.println("Door is open!");
    door_open();
  }

  // we are done here. go back to sleep.
  Serial.println("zzzz");
  ESP.deepSleep(SLEEP_LENGTH * 1000000, WAKE_RF_DEFAULT);
}

// noop
void loop() {}

// connect to wifi network. used by
// door and battery functions.
void wifi_init() {
  // wifi init
  Serial.println("Starting WiFi");
  WiFi.begin(WLAN_SSID, WLAN_PASS);

  // wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    Serial.write('.');
    delay(1000);
  }

  // AIO init
  Serial.println("Connecting to Adafruit.io");
  aio.begin();

}

void door_open() {

  // turn on wifi if we aren't connected
  if(WiFi.status() != WL_CONNECTED) {
    wifi_init();
  }
  
  // grab the door feed
  Adafruit_IO_Feed door = aio.getFeed("door");

  Serial.println("Sending to Adafruit.io");
  // send door open value to AIO
  door.send("1");

}

void battery_level() {

  int rawLevel = analogRead(A0);

    // the 10kΩ/47kΩ voltage divider reduces the voltage, so the ADC Pin can handle it
    // According to Wolfram Alpha, this results in the following values:
    // 10kΩ/(47kΩ+10kΩ)*  5v = 0.8772v
    // 10kΩ/(47kΩ+10kΩ)*3.7v = 0.649v
    // 10kΩ/(47kΩ+10kΩ)*3.1v = 0.544
    // * i asumed 3.1v as minimum voltage => see LiPO discharge diagrams
    // the actual minimum i've seen was 467, which would be 2.7V immediately before automatic cutoff
    // a measurement on the LiPo Pins directly resulted in >3.0V, so thats good to know, but no danger to the battery.

    // convert battery level to percent
    int level = map(rawLevel, 500, 649, 0, 100);

    // i'd like to report back the real voltage, so apply some math to get it back
    // 1. convert the ADC level to a float
    // 2. divide by (R2[1] / R1 + R2)
    // [1] the dot is a trick to handle it as float
    float realVoltage = (float)rawLevel / 1000 / (10000. / (47000 + 10000));
    
    // build a nice string to send to influxdb or whatever you like
    char dataLine[64];
    // sprintf has no support for floats, but will be added later, so we need a String() for now
    sprintf(dataLine, "voltage percent=%d,adc=%d,real=%s,charging=%d\n",
        level < 150 ? level : 100, // cap level to 100%, just for graphing, i don't want to see your lab, when the battery actually gets to that level
        rawLevel,
        String(realVoltage, 3).c_str(),
        rawLevel > 800 ? 1 : 0 // USB is connected if the reading is ~870, as the voltage will be 5V, so we assume it's charging
    );
    Serial.println(dataLine);

    
  // turn on wifi if we aren't connected
  if(WiFi.status() != WL_CONNECTED)
    wifi_init();

  // grab the battery feed
  Adafruit_IO_Feed battery = aio.getFeed("battery");

  // send battery level to AIO
  battery.send(String(realVoltage, 3).c_str());
}