#ifndef DroneConfig_h
#define DroneConfig_h

#include "Arduino.h"
#include <Esp.h>
#include <NTPClient.h>
#include <Wire.h>
#include "BlueDot_BME280_TSL2591.h"

#define VARIANT "droneAir"
#define USE_SERIAL Serial
#define CURRENT_VERSION VERSION
#define CLOUD_FUNCTION_URL "http://europe-west2-drone-302200.cloudfunctions.net/getFirmwareDownloadUrl"
#define DroneDevice_debug false
#define _ChipId ESP.getChipId() 

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

String idPath = "/data/id/";
String CAPath = "/data/security/";
String primaryCA = CAPath + "gtsltsr.crt";
String backupCA = CAPath + "GSR4.crt";

String privateKeyPath = idPath + String(_ChipId) + "_private.der";
String deviceQRPath = idPath + String(_ChipId) + "_qr.png";

// Time (seconds) to expire token += 20 minutes for drift
const int jwt_exp_secs = 36000; // Maximum 24H (3600*24)

const char* onlineStateTopic = "/online-state";
const char* stateTopic = "/deviceStateTopic";

const char* deviceBootTopic = "/deviceBoot";
const char* sensorReadingTopic = "/sensorReading";

BlueDot_BME280_TSL2591 bme280;
BlueDot_BME280_TSL2591 tsl2591;

bool justBoot = true;
uint32_t next_step_time = 0;

#endif
