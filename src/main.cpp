#define VARIANT "esp8266"
#define USE_SERIAL Serial
#define CURRENT_VERSION VERSION
#define CLOUD_FUNCTION_URL "http://eu-gcloud-ota-update-dronemonitor.cloudfunctions.net/getDownDroneAirLoadUrl"

#include <DroneWiFiConnect.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <CloudIoTCore.h>
#include "droneGoogle.h"

WiFiConnect wc;

// For internet connection
WiFiClient client;
HTTPClient http;

void configModeCallback(WiFiConnect *mWiFiConnect) {
  Serial.println(F("Entering Access Point"));
}


void startWiFi(boolean showParams = false) {

  wc.setDebug(true);

  /* Set our callbacks */
  wc.setAPCallback(configModeCallback);

  //wc.resetSettings(); //helper to remove the stored wifi connection, comment out after first upload and re upload

  /*
     AP_NONE = Continue executing code
     AP_LOOP = Trap in a continuous loop - Device is useless
     AP_RESET = Restart the chip
     AP_WAIT  = Trap in a continuous loop with captive portal until we have a working WiFi connection
  */
  if (!wc.autoConnect()) { // try to connect to wifi
    /* We could also use button etc. to trigger the portal on demand within main loop */
    wc.startConfigurationPortal(AP_WAIT);//if not connected show the configuration portal
  }
}

/* 
 * Check if needs to update the device and returns the download url.
 */
String getDownloadUrl()
{
  String downloadUrl;
  USE_SERIAL.print("[HTTP] begin...\n");

  String url = CLOUD_FUNCTION_URL;
  url += String("?version=") + CURRENT_VERSION;
  url += String("&variant=") + VARIANT;
  http.begin(client, url);

  USE_SERIAL.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();

  // httpCode will be negative on error
  if (httpCode > 0)
  {
    // HTTP header has been send and Server response header has been handled
    USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK)
    {
      String payload = http.getString();
      USE_SERIAL.println(payload);
      downloadUrl = payload;
    } else {
      USE_SERIAL.println("Device is up to date!");
    }
  }
  else
  {
    USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();

  return downloadUrl;
}

/* 
 * Download binary image and use Update library to update the device.
 */
bool downloadUpdate(String url)
{
  USE_SERIAL.print("[HTTP] Download begin...\n");

  http.begin(client, url);

  USE_SERIAL.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();
  if (httpCode > 0)
  {
    // HTTP header has been send and Server response header has been handled
    USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK)
    {

      int contentLength = http.getSize();
      USE_SERIAL.println("contentLength : " + String(contentLength));

      if (contentLength > 0)
      {
        bool canBegin = Update.begin(contentLength);
        if (canBegin)
        {
          WiFiClient stream = http.getStream();
          USE_SERIAL.println("Begin OTA. This may take 2 - 5 mins to complete. Things might be quite for a while.. Patience!");
          size_t written = Update.writeStream(stream);

          if ((int) written == (int) contentLength)
          {
            USE_SERIAL.println("Written : " + String(written) + " successfully");
          }
          else
          {
            USE_SERIAL.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?");
          }

          if (Update.end())
          {
            USE_SERIAL.println("OTA done!");
            if (Update.isFinished())
            {
              USE_SERIAL.println("Update successfully completed. Rebooting.");
              ESP.restart();
              return true;
            }
            else
            {
              USE_SERIAL.println("Update not finished? Something went wrong!");
              return false;
            }
          }
          else
          {
            USE_SERIAL.println("Error Occurred. Error #: " + String(Update.getError()));
            return false;
          }
        }
        else
        {
          USE_SERIAL.println("Not enough space to begin OTA");
          client.flush();
          return false;
        }
      }
      else
      {
        USE_SERIAL.println("There was no content in the response");
        client.flush();
        return false;
      }
    }
    else
    {
      return false;
    }
  }
  else
  {
    return false;
  }
}


void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);  
  
  delay (5000);
  if (!LittleFS.begin()) {
    Serial.println(F("An Error has occurred while mounting SPIFFS"));
    return;
  }

  wc.setDeviceId(device_id);
  startWiFi();
 
  if (WiFi.status() == WL_CONNECTED) {
    timeClient.begin();
    setupCloudIoT();
  }

  sensors = new DroneSensorAir(WiFi.macAddress(), WiFi.localIP().toString(), device_id, false);
 
  
  delay(3000);
  Serial.println("\n Starting");
  // Setup Wifi Manager
  
  
  // Check if we need to download a new version
  String downloadUrl = getDownloadUrl();
  if (downloadUrl.length() > 0)
  {
    bool success = downloadUpdate(downloadUrl);
    if (!success)
    {
      USE_SERIAL.println("Error updating device");
    }
  }

 }


void loop() {
 
  // Wifi Dies? Start Portal Again
  if (WiFi.status() != WL_CONNECTED) {
    if (!wc.autoConnect()) wc.startConfigurationPortal(AP_WAIT);
  }
  else
  {
    if (!mqtt->loop())
    {
      mqtt->mqttConnect();
    }

    if (justBoot) {
      sensors->DroneSensor_debug ? Serial.println(F("Debuggind On")) : Serial.println(F("Debuggind Off"));
      processBoot();
      processState();
      justBoot = false;
      next_step_time = millis()+ sensors->pollDelay;
    }

    if (millis() >= next_step_time) {
      processSensor();
      next_step_time = millis() + sensors->pollDelay;
    }
  }
}
