#define BLYNK_TEMPLATE_ID "TMPLxxxxxxx"
#define BLYNK_TEMPLATE_NAME "ColdStorage"
#define BLYNK_AUTH_TOKEN "xxxxxxxxxxxxxxxxxxxxxxxx"
/****************************************************
 IoT Based Real-Time Cold Storage Monitoring System

 Author : Hitha M

 Hardware:
 - ESP32
 - DHT11
 - MQ135
 - Moisture Sensor
 - LDR
 - GPS Neo-6M
 - GSM SIM900A
 - Relay Module

****************************************************/

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>

#include "config.h"

//=============================
// Create Objects
//=============================

DHT dht(DHTPIN, DHTTYPE);

TinyGPSPlus gps;

HardwareSerial gpsSerial(1);
HardwareSerial gsmSerial(2);

//=============================
// Global Variables
//=============================

float temperature = 0;
float humidity = 0;

int gasValue = 0;
int moistureValue = 0;
int lightValue = 0;

double latitude = 0;
double longitude = 0;

//=============================
// Function Declaration
//=============================

void connectWiFi();
void initializePins();
void readSensors();
void uploadToBlynk();
void checkThresholds();
void controlRelays();
void sendSMS(String msg);
void readGPS();
//=====================================
// Setup Function
//=====================================

void setup()
{
    Serial.begin(115200);

    dht.begin();

    initializePins();

    connectWiFi();

    gpsSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);

    gsmSerial.begin(9600, SERIAL_8N1, GSM_RX, GSM_TX);

    Blynk.begin(BLYNK_AUTH_TOKEN,
                WIFI_SSID,
                WIFI_PASSWORD);

    Serial.println("--------------------------------");
    Serial.println("Cold Storage Monitoring Started");
    Serial.println("--------------------------------");
}

//=====================================
// Main Loop
//=====================================

void loop()
{
    Blynk.run();

    readSensors();

    readGPS();

    checkThresholds();

    uploadToBlynk();

    controlRelays();

    delay(2000);
}
//=====================================
// Initialize GPIO Pins
//=====================================

void initializePins()
{
    pinMode(MQ135_PIN, INPUT);
    pinMode(MOISTURE_PIN, INPUT);
    pinMode(LDR_PIN, INPUT);

    pinMode(COOLING_RELAY, OUTPUT);
    pinMode(FAN_RELAY, OUTPUT);
    pinMode(ALARM_RELAY, OUTPUT);
    pinMode(LIGHT_RELAY, OUTPUT);

    digitalWrite(COOLING_RELAY, HIGH);
    digitalWrite(FAN_RELAY, HIGH);
    digitalWrite(ALARM_RELAY, HIGH);
    digitalWrite(LIGHT_RELAY, HIGH);
}
//=====================================
// Connect ESP32 to WiFi
//=====================================

void connectWiFi()
{
    Serial.print("Connecting to WiFi");

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi Connected Successfully");
    Serial.print("IP Address : ");
    Serial.println(WiFi.localIP());
}
//=====================================
// Read All Sensors
//=====================================

void readSensors()
{
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();

    gasValue = analogRead(MQ135_PIN);

    moistureValue = analogRead(MOISTURE_PIN);

    lightValue = digitalRead(LDR_PIN);

    Serial.println("-------------------------");

    Serial.print("Temperature : ");
    Serial.println(temperature);

    Serial.print("Humidity : ");
    Serial.println(humidity);

    Serial.print("Gas Value : ");
    Serial.println(gasValue);

    Serial.print("Moisture : ");
    Serial.println(moistureValue);

    Serial.print("Door Status : ");

    if(lightValue==LOW)
        Serial.println("OPEN");
    else
        Serial.println("CLOSED");
}
//=====================================
// Upload Sensor Values to Blynk
//=====================================

void uploadToBlynk()
{
    Blynk.virtualWrite(V0, temperature);
    Blynk.virtualWrite(V1, humidity);
    Blynk.virtualWrite(V2, gasValue);
    Blynk.virtualWrite(V3, moistureValue);
    Blynk.virtualWrite(V4, lightValue);
}
//=====================================
// Read GPS Coordinates
//=====================================

void readGPS()
{
    while (gpsSerial.available())
    {
        gps.encode(gpsSerial.read());
    }

    if (gps.location.isUpdated())
    {
        latitude = gps.location.lat();
        longitude = gps.location.lng();

        Serial.print("Latitude : ");
        Serial.println(latitude, 6);

        Serial.print("Longitude : ");
        Serial.println(longitude, 6);
    }
}
//=====================================
// Relay Control
//=====================================

void controlRelays()
{
    // Cooling and Fan Control
    if (temperature > TEMP_MAX)
    {
        digitalWrite(COOLING_RELAY, LOW);
        digitalWrite(FAN_RELAY, LOW);
    }
    else
    {
        digitalWrite(COOLING_RELAY, HIGH);
        digitalWrite(FAN_RELAY, HIGH);
    }

    // Door Light Control
    if (lightValue == LOW)
    {
        digitalWrite(LIGHT_RELAY, LOW);
    }
    else
    {
        digitalWrite(LIGHT_RELAY, HIGH);
    }

    // Alarm Control
    if (temperature > TEMP_MAX ||
        gasValue > GAS_THRESHOLD ||
        moistureValue > MOISTURE_HIGH)
    {
        digitalWrite(ALARM_RELAY, LOW);
    }
    else
    {
        digitalWrite(ALARM_RELAY, HIGH);
    }
}
//=====================================
// Check Sensor Thresholds
//=====================================

void checkThresholds()
{
    String alertMessage = "";

    if (temperature > TEMP_MAX)
    {
        alertMessage += "High Temperature Detected!\n";
    }

    if (temperature < TEMP_MIN)
    {
        alertMessage += "Low Temperature Detected!\n";
    }

    if (humidity > HUM_MAX)
    {
        alertMessage += "High Humidity!\n";
    }

    if (humidity < HUM_MIN)
    {
        alertMessage += "Low Humidity!\n";
    }

    if (gasValue > GAS_THRESHOLD)
    {
        alertMessage += "Gas Leakage Detected!\n";
    }

    if (moistureValue > MOISTURE_HIGH)
    {
        alertMessage += "High Moisture Detected!\n";
    }

    if (lightValue == LOW)
    {
        alertMessage += "Cold Storage Door Open!\n";
    }

    if (alertMessage.length() > 0)
    {
        sendSMS(alertMessage);
    }
}
//=====================================
// Send SMS using GSM Module
//=====================================

void sendSMS(String message)
{
    Serial.println("Sending SMS...");

    gsmSerial.println("AT");
    delay(1000);

    gsmSerial.println("AT+CMGF=1");
    delay(1000);

    gsmSerial.println("AT+CMGS=\"+911234567890\"");
    delay(1000);

    gsmSerial.print(message);

    gsmSerial.write(26);

    delay(3000);

    Serial.println("SMS Sent Successfully");
}
