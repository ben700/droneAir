
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
    Serial.begin(9600);
  
 
  bme280.parameter.I2CAddress = 0x77;                 //The BME280 is hardwired to use the I2C Address 0x77              
  tsl2591.parameter.I2CAddress = 0x29; 


//0b00:    Low gain mode
   //0b01:    Medium gain mode
   //0b10:    High gain mode
   //0b11:    Maximum gain mode

   tsl2591.parameter.gain = 0b01;

   //Longer integration times also helps in very low light situations, but the measurements are slower
 
   //0b000:   100ms (max count = 37888)
   //0b001:   200ms (max count = 65535)
   //0b010:   300ms (max count = 65535)
   //0b011:   400ms (max count = 65535)
   //0b100:   500ms (max count = 65535)
   //0b101:   600ms (max count = 65535)
    
   tsl2591.parameter.integration = 0b000;    

   //The values for the gain and integration times are written transfered to the sensor through the function config_TSL2591
   //This function powers the device ON, then configures the sensor and finally powers the device OFF again 
       
   tsl2591.config_TSL2591();  

   
  
  //*********************************************************************
  //*************ADVANCED SETUP - SAFE TO IGNORE!************************
  
  //Now choose on which mode your device will run
  //On doubt, just leave on normal mode, that's the default value

  //0b00:     In sleep mode no measurements are performed, but power consumption is at a minimum
  //0b01:     In forced mode a single measured is performed and the device returns automatically to sleep mode
  //0b11:     In normal mode the sensor measures continually (default value)
  
    bme280.parameter.sensorMode = 0b11;                   //Choose sensor mode

    

  //*********************** TSL2591 *************************************
  //*************ADVANCED SETUP - SAFE TO IGNORE!************************
  
  //Great! Now set up the internal IIR Filter
  //The IIR (Infinite Impulse Response) filter suppresses high frequency fluctuations
  //In short, a high factor value means less noise, but measurements are also less responsive
  //You can play with these values and check the results!
  //In doubt just leave on default

  //0b000:      factor 0 (filter off)
  //0b001:      factor 2
  //0b010:      factor 4
  //0b011:      factor 8
  //0b100:      factor 16 (default value)

    bme280.parameter.IIRfilter = 0b100;                    //Setup for IIR Filter

    

  //************************** BME280 ***********************************
  //*************ADVANCED SETUP - SAFE TO IGNORE!************************
  
  //Next you'll define the oversampling factor for the humidity measurements
  //Again, higher values mean less noise, but slower responses
  //If you don't want to measure humidity, set the oversampling to zero

  //0b000:      factor 0 (Disable humidity measurement)
  //0b001:      factor 1
  //0b010:      factor 2
  //0b011:      factor 4
  //0b100:      factor 8
  //0b101:      factor 16 (default value)
  
    bme280.parameter.humidOversampling = 0b101;            //Setup Humidity Oversampling

    

  //************************** BME280 ***********************************
  //*************ADVANCED SETUP - SAFE TO IGNORE!************************
  
  //Now define the oversampling factor for the temperature measurements
  //You know now, higher values lead to less noise but slower measurements
  
  //0b000:      factor 0 (Disable temperature measurement)
  //0b001:      factor 1
  //0b010:      factor 2
  //0b011:      factor 4
  //0b100:      factor 8
  //0b101:      factor 16 (default value)

    bme280.parameter.tempOversampling = 0b101;             //Setup Temperature Ovesampling

    

  //************************** BME280 ***********************************
  //*************ADVANCED SETUP - SAFE TO IGNORE!************************
  
  //Finally, define the oversampling factor for the pressure measurements
  //For altitude measurements a higher factor provides more stable values
  //On doubt, just leave it on default
  
  //0b000:      factor 0 (Disable pressure measurement)
  //0b001:      factor 1
  //0b010:      factor 2
  //0b011:      factor 4
  //0b100:      factor 8
  //0b101:      factor 16 (default value)
  
    bme280.parameter.pressOversampling = 0b101;            //Setup Pressure Oversampling 

    
  
  //************************** BME280 ***********************************
  //*************ADVANCED SETUP - SAFE TO IGNORE!************************
  
  //For precise altitude measurements please put in the current pressure corrected for the sea level
  //On doubt, just leave the standard pressure as default (1013.25 hPa)
  
    bme280.parameter.pressureSeaLevel = 1013.25;           //default value of 1013.25 hPa

  //Now write here the current average temperature outside (yes, the outside temperature!)
  //You can either use the value in Celsius or in Fahrenheit, but only one of them (comment out the other value)
  //In order to calculate the altitude, this temperature is converted by the library into Kelvin
  //For slightly less precise altitude measurements, just leave the standard temperature as default (15°C)
  //Remember, leave one of the values here commented, and change the other one!
  //If both values are left commented, the default temperature of 15°C will be used
  //But if both values are left uncommented, then the value in Celsius will be used    
  
    bme280.parameter.tempOutsideCelsius = 15;              //default value of 15°C
  //bme280.parameter.tempOutsideFahrenheit = 59;           //default value of 59°F


  
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
 
    if (bme280.init_BME280() != 0x60)  
  {        
    Serial.println(F("Ops! BME280 could not be found!"));
    //while(1);
  }
  else
  {
    Serial.println(F("BME280 detected!"));
  }

  if (tsl2591.init_TSL2591() != 0x50)  
  {        
    Serial.println(F("Ops! TSL2591 could not be found!"));
    //while(1);
  }
  else
  {
    Serial.println(F("TSL2591 detected!"));
  }
  
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

    if (bme280.init_BME280() != 0x60)  
  {        
    Serial.println(F("Ops! BME280 could not be found!"));
    //while(1);
  }
  else
  {
    Serial.println(F("BME280 detected!"));
  }

  if (tsl2591.init_TSL2591() != 0x50)  
  {        
    Serial.println(F("Ops! TSL2591 could not be found!"));
    //while(1);
  }
  else
  {
    Serial.println(F("TSL2591 detected!"));
  }
  
  Serial.println();
  Serial.println(); 

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

           
   Serial.print(F("Duration in Seconds:\t\t"));
   Serial.println(float(millis())/1000);
 
   Serial.print(F("Temperature in Celsius:\t\t")); 
   Serial.println(bme280.readTempC());
   
   Serial.print(F("Humidity in %:\t\t\t")); 
   Serial.println(bme280.readHumidity());

   Serial.print(F("Pressure in hPa:\t\t")); 
   Serial.println(bme280.readPressure());

   Serial.print(F("Altitude in Meters:\t\t")); 
   Serial.println(bme280.readAltitudeMeter());

   Serial.print(F("Illuminance in Lux:\t\t")); 
   Serial.println(tsl2591.readIlluminance_TSL2591());

   Serial.println();
   Serial.println();



      processSensor();
      next_step_time = millis() + 60000;
    }
  }
}
