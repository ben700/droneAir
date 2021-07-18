
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <CloudIoTCore.h>
#include "droneGoogle.h"
#include <Wire.h>


WiFiManager wifiManager;
// For internet connection
WiFiClient client;
HTTPClient http;

/* 
 * Check if needs to update the device and returns the download url.
 */
String getDownloadUrl()
{
  String downloadUrl;
  USE_SERIAL.print("[HTTP] begin...\n");

  String url = CLOUD_FUNCTION_URL;
  url += String("?version=") + CURRENT_VERSION;
  url += String("&variant=") + device_type;
  http.begin(client, url);

USE_SERIAL.print("CLOUD_FUNCTION_URL = ");
USE_SERIAL.println(url);

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
      String payload = http.getString();
      USE_SERIAL.println(payload);

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
  

  

wifiManager.setAPStaticIPConfig(IPAddress(4,20,4,20), IPAddress(4,20,4,20), IPAddress(255,255,255,0));
wifiManager.autoConnect(device_id);

 
  if (WiFi.status() == WL_CONNECTED) {
    timeClient.begin();
    setupCloudIoT();
  }

  Wire.begin();
  sensor.begin(); // reset sensor
  
  
  
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
    wifiManager.autoConnect(device_id);
  }
  else
  {
    if (!mqtt->loop())
    {
      mqtt->mqttConnect();
    }
    delay(10); 
  
    if (millis() >= next_step_time) {
      processSensor();
      next_step_time = millis() + 60000;
    }
  }
}
