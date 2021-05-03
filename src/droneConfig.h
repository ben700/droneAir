#ifndef DroneConfig_h
#define DroneConfig_h

#include "Arduino.h"
#include "DroneSensorAir.h"
#include <NTPClient.h>
#define VARIANT "droneAir"
#define USE_SERIAL Serial
#define CURRENT_VERSION VERSION
#define CLOUD_FUNCTION_URL "http://europe-west2-drone-302200.cloudfunctions.net/getFirmwareDownloadUrl"
#define _ChipId ESP.getChipId() 
// Configuration for NTP
// Define NTP Client to get time
const char* ntp_primary = "time.google.com";
const char* ntp_secondary = "time1.google.com";
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

const char* project_id = "drone-302200";
const char* location = "europe-west1";
const char* registry_id = "droneDevice";
const char* device_type = VARIANT;
String deviceId = String(VARIANT) + String(_ChipId);
const char* device_id = deviceId.c_str();
// Time (seconds) to expire token += 20 minutes for drift
const int jwt_exp_secs = 36000; // Maximum 24H (3600*24)


const char* deviceBootTopic = "/deviceBoot";
const char* sensorReadingTopic = "/sensorReading";

DroneSensorAir *sensors;
bool justBoot = true;
uint32_t next_step_time = 0;

#endif
