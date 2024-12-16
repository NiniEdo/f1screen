# F1 Screen
### (Still WIP)
## Description
F1 Screen is an Arduino-based project that displays Formula 1 race information on an e-paper display. Use the Ergast API to get up-to-date data on races, practice sessions, qualifying and the races themselves. Inspired by SurvivalHacking

## Characteristics
Viewing information on Formula 1 races
Support for practice sessions, qualifications and races
Automatic data update via API
Using an e-paper display for clear, low-power viewing

## Requirements:
PlatformIO
ArduinoJson (version 7.2.0 or higher)
GxEPD2 (version 1.6.0 or higher)
Now (version 1.6.1 or higher)
An Arduino compatible device (e.g. ESP32)
An e-paper compatible display (for example, GxEPD2_213_BN)

## Installation:
Clone this repository, install dependencies, configure WiFi credentials in the wifi_credentials.h file and set your timezone in the utils.cpp file, upload the project to your device

## Usage
The device will connect to the configured WiFi network and begin downloading Formula 1 race data.
Information on practice sessions, qualifying and races will be stored on the e-paper display.
Currently the display has two screens: 
1) During the race week it shows the track layout and the dates and times for every session.![raceweek](https://github.com/user-attachments/assets/c99f0bd4-12a9-43be-9d6e-e3e1c6df9bae)

2) During non-raceweek days it shows the next two races dates and the championship leaderboard
![home](https://github.com/user-attachments/assets/3e29eb3e-f02d-4ad4-93ca-9d211663c732)

The esp32 hibernates after the data is displayes and updates every day at midnight

## BOM: 

Screen: https://it.aliexpress.com/item/1005004644515880.html?aff_fcid=f46fabb132214bfd894bc9e95eaabc0b-1731597692108-09062-_DkVTN0b&tt=CPS_NORMAL&aff_fsk=_DkVTN0b&aff_platform=shareComponent-detail&sk=_DkVTN0b&aff_trace_key=f46fabb132214bfd894bc9e95eaabc0b-1731597692108-09062-_DkVTN0b&terminal_id=21c481ea1fd849c8a0215a3e6e7d2b2f&afSmartRedirect=y 

ESP32: https://it.aliexpress.com/item/1005007144383095.html?aff_fcid=9810f47ca0d54394a703c003dca07f17-1731597720870-02766-_Dl6aoUX&tt=CPS_NORMAL&aff_fsk=_Dl6aoUX&aff_platform=shareComponent-detail&sk=_Dl6aoUX&aff_trace_key=9810f47ca0d54394a703c003dca07f17-1731597720870-02766-_Dl6aoUX&terminal_id=21c481ea1fd849c8a0215a3e6e7d2b2f&afSmartRedirect=y 

Battery: Standard 18650 battery
