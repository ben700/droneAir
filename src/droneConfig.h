#ifndef DroneConfig_h
#define DroneConfig_h

#include "Arduino.h"
#include <Esp.h>
#include <NTPClient.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_TSL2591.h>
#include <MHZ.h>
#include "DroneSensorAir.h"


#define _ChipId ESP.getChipId() 
// pin for pwm reading
#define CO2_IN 10

// pin for uart reading
#define MH_Z19_RX D4 // D7
#define MH_Z19_TX D0 // D6

#define SEALEVELPRESSURE_HPA (1013.25)


// Configuration for NTP
// Define NTP Client to get time
const char* ntp_primary = "time.google.com";
const char* ntp_secondary = "time1.google.com";
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

const char* project_id = "drone-302200";
const char* location = "europe-west1";
const char* registry_id = "droneDeviceProduction";
const char* device_type = VARIANT;
String deviceId = String(VARIANT) +"_"+ String(_ChipId);
const char* device_id = deviceId.c_str();

String IDPath = "/data/id/";
String CAPath = "/data/security/";
String primaryCA = CAPath + "gtsltsr.crt";
String backupCA = CAPath + "GSR4.crt";

String privateKeyPath = IDPath + String(_ChipId) + "_private.der";
String deviceQRPath = IDPath + String(_ChipId) + "_qr.png";


// Time (seconds) to expire token += 20 minutes for drift
const int jwt_exp_secs = 36000; // Maximum 24H (3600*24)

const char *onlineStateTopic = "/online-state";
const char *sensorReadingTopic = "/sensor-reading";
const char *stateTopic = "/";
const char *sensorConfigTopic = "/sensor-config";
const char *deviceExceptionTopic = "/device-exception";

Adafruit_BME280 bme280;
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591);
MHZ co2(MH_Z19_RX, MH_Z19_TX, CO2_IN, MHZ19B);
DroneSensorAir *sensors;

bool justBoot = true;
uint32_t next_step_time = 0;

#endif
