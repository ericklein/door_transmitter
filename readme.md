# door_transmitter

Description: Transmits door open/close state to Adafruit IO (cloud)

Sources
  - ESP8266 Arduino from: https://github.com/esp8266/Arduino

Target
  - tested on Adafruit Feather Huzzah 8266 https://www.adafruit.com/product/2471
  - reed switch with NO and NC

Revisions
	05/15/18
  		-	fork from Adafruit IO door tracking example (https://learn.adafruit.com/using-ifttt-with-adafruit-io/), Written by Todd Treece for Adafruit Industries. MIT license, all text above must be included in any redistribution
  		- removed EEPROM code
  		- switched to ESP.DeepSleep(0) and sensor driven wake from deep sleep
  		-	fixed bug in example code related to 8266 DeepSleep (seconds in exponential format)
	10/17/18 - reverted to original example code (wake at x second interval) to capture both open and closed door states without additional hardware
	10/21/18
		- code cleanup
		- fixed battery code
    
Feature Requests
	- 10/20/18 - move IO feed name from code to config.h	

Information Sources
Adafruit Huzzah pinout - https://learn.adafruit.com/adafruit-huzzah-esp8266-breakout/pinouts
Feather Huzzah pinout - https://learn.adafruit.com/assets/46249
INA219 High Side DC Current sensor - https://www.adafruit.com/product/904
Batteries - https://www.adafruit.com/category/44_138
http://hex.ro/wp/blog/feather-huzzah-esp8266-realistic-power-consumption/
https://github.com/lobeck/adafruit-feather-huzzah-8266-battery-monitor/blob/master/main.ino
https://tinycircuits.com/blogs/learn/121003015-tinyserver-door-monitor-tutorial
https://openhomeautomation.net/esp8266-battery/
https://github.com/esp8266/Arduino/issues/1729