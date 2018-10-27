// Library initialization
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "Adafruit_IO_Client.h"

// function prototypes
void battery_level();
void door_open();

// Assign Arduino pins
#define DOOR 13
// // Assign Arduino pins
#define DOOR 3

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
//#define WLAN_PASS       "coolhardware2017"
// Adafruit IO access key
#define AIO_KEY         "7bda6629571247bda1c3262740a93293"

@ -37,48 +32,17 @@ Adafruit_IO_Client aio = Adafruit_IO_Client(client, AIO_KEY);

void setup() {
  Serial.begin(115200);
  Serial.println("HUZZAH Trigger Basic");
  Serial.println("Door app started");
  
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
send_status();
//ESP.deepSleep(0);
}

// noop
void loop() {}
void loop() {
  Serial.print("door value is ");
  Serial.println(digitalRead(DOOR));
  }

// connect to wifi network. used by
// door and battery functions.
@ -99,7 +63,9 @@ void wifi_init() {

}

void door_open() {
void send_status() {

  int rawLevel = analogRead(A0);

  // turn on wifi if we aren't connected
  if(WiFi.status() != WL_CONNECTED) {
@ -113,12 +79,6 @@ void door_open() {
  // send door open value to AIO
  door.send("1");

}

void battery_level() {

  int rawLevel = analogRead(A0);

    // the 10kΩ/47kΩ voltage divider reduces the voltage, so the ADC Pin can handle it
    // According to Wolfram Alpha, this results in the following values:
    // 10kΩ/(47kΩ+10kΩ)*  5v = 0.8772v
@ -148,11 +108,6 @@ void battery_level() {
    );
    Serial.println(dataLine);

    
  // turn on wifi if we aren't connected
  if(WiFi.status() != WL_CONNECTED)
    wifi_init();

  // grab the battery feed
  Adafruit_IO_Feed battery = aio.getFeed("battery");
