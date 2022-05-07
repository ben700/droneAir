#ifndef DroneCloudOTA_h
#define DroneCloudOTA_h

#define CLOUD_FUNCTION_URL "http://europe-west2-drone-302200.cloudfunctions.net/getFirmwareDownloadUrl"

#include "Arduino.h"
#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>


class DroneCloudOTA
{
public:
  DroneCloudOTA();
  ~DroneCloudOTA();

  bool reinstall();
  bool update(bool force = false);
  
  String setURL(bool force = false);
 

private:
  String url;

  String downloadFirmwareBackupUrl;
  String downloadUrl;
//  DroneDebug* droneDebug;
};

#endif
