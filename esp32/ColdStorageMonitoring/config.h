#ifndef CONFIG_H
#define CONFIG_H

//=============================
// WiFi Credentials
//=============================

#define WIFI_SSID      "YOUR_WIFI_NAME"
#define WIFI_PASSWORD  "YOUR_WIFI_PASSWORD"

//=============================
// Blynk Credentials
//=============================


//=============================
// DHT11
//=============================

#define DHTPIN 4
#define DHTTYPE DHT11

//=============================
// Analog Sensors
//=============================

#define MQ135_PIN      34
#define MOISTURE_PIN   35
#define LDR_PIN        32

//=============================
// Relay Pins
//=============================

#define COOLING_RELAY  25
#define FAN_RELAY      26
#define ALARM_RELAY    27
#define LIGHT_RELAY    14

//=============================
// GPS
//=============================

#define GPS_RX 16
#define GPS_TX 17

//=============================
// GSM
//=============================

#define GSM_RX 18
#define GSM_TX 19

//=============================
// Thresholds
//=============================

#define TEMP_MIN 2
#define TEMP_MAX 8

#define HUM_MIN 50
#define HUM_MAX 80

#define GAS_THRESHOLD 2000

#define MOISTURE_HIGH 3000
#define MOISTURE_LOW 700

#endif