#include <Arduino.h>

#include <droneMRD.h>
#include <droneCloudOTA.h>
#include <droneDebug.h>

#include <ESP_WiFiManager.h> //https://github.com/khoih-prog/ESP_WiFiManager
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <CloudIoTCore.h>
#include <EasyLed.h>
#include <Wire.h>
#include <i2cdetect.h>

#include "droneGoogle.h"
#include "droneConfig.h"
#include "sensorConfig.h"

// for LED status
EasyLed led(LED_BUILTIN, EasyLed::ActiveLevel::Low, EasyLed::State::On);

ESP_WiFiManager ESP_wifiManager;

DroneCloudOTA *droneCloudOTA;
WiFiClient client;
HTTPClient http;
DroneDebug *droneDebug;

bool _hasSensors = false;
bool _detectReset = false;

void setup()
{

  Serial.begin(115200);
  while (!Serial)
    ;

  delay(200);

droneDebug = new DroneDebug("droneAir");

  droneDebug->DEBUG_WM(F("Starting ConfigOnDoubleReset_Multi with DoubleResetDetect"));
  droneDebug->DEBUG_WM("Using " + String(FS_Name));
  droneDebug->DEBUG_WM("On " + String(ARDUINO_BOARD));
  droneDebug->DEBUG_WM(ESP_WIFIMANAGER_VERSION);
  droneDebug->DEBUG_WM(ESP_MULTI_RESET_DETECTOR_VERSION);

  if (!LittleFS.begin())
  {
    droneDebug->DEBUG_WM(F("An Error has occurred while mounting LittleFS"));
    return;
  }
  led.flash(4, 400, 200);

  mrd = new MultiResetDetector(MRD_TIMEOUT, MRD_ADDRESS);
  unsigned long startedAt = millis();

  initAPIPConfigStruct(WM_AP_IPconfig);
  initSTAIPConfigStruct(WM_STA_IPconfig);
  ESP_WiFiManager ESP_wifiManager("droneDevice");
  ESP_wifiManager.setAPStaticIPConfig(WM_AP_IPconfig);

  ESP_wifiManager.setMinimumSignalQuality(-1);
  ESP_wifiManager.setConfigPortalChannel(0);
  ESP_wifiManager.setSTAStaticIPConfig(WM_STA_IPconfig);
  ESP_wifiManager.setCORSHeader("Your Access-Control-Allow-Origin");

  // ESP_wifiManager.setCustomHeadElement("<style>html{filter: invert(50%); -webkit-filter: invert(50%);}</style>");
  ESP_WMParameter custom_text("<p>This is Droneponics Device Manager WiFi Manager</p>");
  ESP_wifiManager.addParameter(&custom_text);

  Router_SSID = ESP_wifiManager.WiFi_SSID();
  Router_Pass = ESP_wifiManager.WiFi_Pass();

  bool configDataLoaded = false;

  // From v1.1.0, Don't permit NULL password
  if ((Router_SSID != "") && (Router_Pass != ""))
  {
    LOGERROR3(F("* Add SSID = "), Router_SSID, F(", PW = "), Router_Pass);
    wifiMulti.addAP(Router_SSID.c_str(), Router_Pass.c_str());

    ESP_wifiManager.setConfigPortalTimeout(120); // If no access point name has been previously entered disable timeout.
    droneDebug->DEBUG_WM(F("Got ESP Self-Stored Credentials. Timeout 120s for Config Portal"));
  }

  if (loadConfigData())
  {
    configDataLoaded = true;

    ESP_wifiManager.setConfigPortalTimeout(120); // If no access point name has been previously entered disable timeout.
    droneDebug->DEBUG_WM(F("Got stored Credentials. Timeout 120s for Config Portal"));
  }
  else
  {
    // Enter CP only if no stored SSID on flash and file
    droneDebug->DEBUG_WM(F("Open Config Portal without Timeout: No stored Credentials."));
    initialConfig = true;
  }

  if (mrd->detectMultiReset())
  {
    // DRD, disable timeout.
    ESP_wifiManager.setConfigPortalTimeout(0);

    droneDebug->DEBUG_WM(F("Multi Reset Detected"));
    initialConfig = true;
    _detectReset = true;

    led.flash(3, 400, 200, 300, 300);
  }
  else
  {
    droneDebug->DEBUG_WM(F("No Multi Reset Detected"));
    droneDebug->DEBUG_WM("Reset need to be reset a number of times = " +  String(MRD_TIMES));
    mrd->detectMultiReset();
    led.off();
  }

  if (initialConfig)
  {
    droneDebug->DEBUG_WM("Starting configuration portal @ " + APStaticIP.toString());

    led.flash(20, 100, 100);

    led.flash(10, 100, 1500);
    led.flash(250, 1500, 100);

    // digitalWrite(PIN_LED, LED_ON); // turn the LED on by making the voltage LOW to tell us we are in configuration mode.

    // sets timeout in seconds until configuration portal gets turned off.
    // If not specified device will remain in configuration mode until
    // switched off via webserver or device is restarted.
    // ESP_wifiManager.setConfigPortalTimeout(600);

    // Starts an access point
    if (!ESP_wifiManager.startConfigPortal((const char *)ssid.c_str(), (const char *)password.c_str()))
      droneDebug->DEBUG_WM(F("Not connected to WiFi but continuing anyway."));
    else
    {
      droneDebug->DEBUG_WM(F("WiFi connected...yeey :)"));
    }

    // Stored  for later usage, from v1.1.0, but clear first
    memset(&WM_config, 0, sizeof(WM_config));

    for (uint8_t i = 0; i < NUM_WIFI_CREDENTIALS; i++)
    {
      String tempSSID = ESP_wifiManager.getSSID(i);
      String tempPW = ESP_wifiManager.getPW(i);

      if (strlen(tempSSID.c_str()) < sizeof(WM_config.WiFi_Creds[i].wifi_ssid) - 1)
        strcpy(WM_config.WiFi_Creds[i].wifi_ssid, tempSSID.c_str());
      else
        strncpy(WM_config.WiFi_Creds[i].wifi_ssid, tempSSID.c_str(), sizeof(WM_config.WiFi_Creds[i].wifi_ssid) - 1);

      if (strlen(tempPW.c_str()) < sizeof(WM_config.WiFi_Creds[i].wifi_pw) - 1)
        strcpy(WM_config.WiFi_Creds[i].wifi_pw, tempPW.c_str());
      else
        strncpy(WM_config.WiFi_Creds[i].wifi_pw, tempPW.c_str(), sizeof(WM_config.WiFi_Creds[i].wifi_pw) - 1);

      // Don't permit NULL SSID and password len < MIN_AP_PASSWORD_SIZE (8)
      if ((String(WM_config.WiFi_Creds[i].wifi_ssid) != "") && (strlen(WM_config.WiFi_Creds[i].wifi_pw) >= MIN_AP_PASSWORD_SIZE))
      {
        LOGERROR3(F("* Add SSID = "), WM_config.WiFi_Creds[i].wifi_ssid, F(", PW = "), WM_config.WiFi_Creds[i].wifi_pw);
        wifiMulti.addAP(WM_config.WiFi_Creds[i].wifi_ssid, WM_config.WiFi_Creds[i].wifi_pw);
      }
    }

    // New in v1.4.0
    ESP_wifiManager.getSTAStaticIPConfig(WM_STA_IPconfig);
    //////

    saveConfigData();
  }

  startedAt = millis();

  if (!initialConfig)
  {
    // Load stored data, the addAP ready for MultiWiFi reconnection
    if (!configDataLoaded)
      loadConfigData();

    for (uint8_t i = 0; i < NUM_WIFI_CREDENTIALS; i++)
    {
      // Don't permit NULL SSID and password len < MIN_AP_PASSWORD_SIZE (8)
      if ((String(WM_config.WiFi_Creds[i].wifi_ssid) != "") && (strlen(WM_config.WiFi_Creds[i].wifi_pw) >= MIN_AP_PASSWORD_SIZE))
      {
        LOGERROR3(F("* Add SSID = "), WM_config.WiFi_Creds[i].wifi_ssid, F(", PW = "), WM_config.WiFi_Creds[i].wifi_pw);
        wifiMulti.addAP(WM_config.WiFi_Creds[i].wifi_ssid, WM_config.WiFi_Creds[i].wifi_pw);
      }
    }

    if (WiFi.status() != WL_CONNECTED)
    {
      droneDebug->DEBUG_WM(F("WiFi not connected"));
      droneDebug->DEBUG_WM(F("ConnectMultiWiFi in setup"));
      led.on();
      connectMultiWiFi();
    }
  }

  droneDebug->DEBUG_WM(F("After waiting "));
  droneDebug->DEBUG_WM(String((millis() - startedAt) / 1000));
  droneDebug->DEBUG_WM(F(" secs more in setup(), connection result is "));

  if (WiFi.status() == WL_CONNECTED)
  {
    droneDebug->DEBUG_WM(F("connected. Local IP: "));
    droneDebug->DEBUG_WM(WiFi.localIP().toString());

    led.flash(3);
    droneCloudOTA = new DroneCloudOTA();
    droneCloudOTA->update(_detectReset);
    led.off();
  }
  else
  {
    droneDebug->DEBUG_WM(ESP_wifiManager.getStatus(WiFi.status()));
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    timeClient.begin();
    if (!setupCloudIoT())
    {
      led.flash(10, 100, 100, 10000, 10000);
      droneDebug->DEBUG_WM(F("Failed to connect with cloud backend"));
      led.flash(250, 100, 100);
      return;
    }
  }

  if (!bme280.begin(0x76, &Wire)) // may be 77
  {
    droneDebug->DEBUG_WM("Could not find a valid BME280 sensor, check wiring!");
  }
  else if (!bme280.begin(0x77, &Wire)) // may be 77
  {
    droneDebug->DEBUG_WM("Could not find a valid BME280 sensor, check wiring!");
  }
  else
  {
    droneDebug->DEBUG_WM("No BME280 sensor found ... check your wiring?");

    i2cdetect();
    led.flash(250, 100, 100);
  }

  if (tsl.begin(&Wire, 0x29))
  {
    droneDebug->DEBUG_WM("Found a TSL2591 sensor");
    printTSL2591SensorDetails();
  }
  else
  {
    droneDebug->DEBUG_WM("No TSL2591 sensor found ... check your wiring?");
    i2cdetect();
    led.flash(250, 200, 200);
  }

  // config sensors

  bme280Config();
  tsl2591Config();
  mhz19bConfig();

  // this class mainly for Atlas sensors
  sensors = new DroneSensorAir(WiFi.macAddress(), WiFi.localIP().toString(), device_id, false);
  sensors->turnParametersOn();
}

void loop()
{
  // put your main code here, to run repeatedly:

  // Wifi Dies? Start Portal Again
  if (WiFi.status() != WL_CONNECTED)
  {
    droneDebug->DEBUG_WM(ESP_wifiManager.getStatus(WiFi.status()));
  }
  else
  {
    if (!mqtt->loop())
    {
      mqtt->mqttConnect();
    }

    if (justBoot)
    {

      processState();
      justBoot = false;
      next_step_time = millis() + sensors->pollDelay;
    }

    if (millis() >= next_step_time)
    {

      bme280PrintValues();
      tsl2591PrintValues();
      mhz19bPrintValues();

      processSensor();
      next_step_time = millis() + sensors->pollDelay;
    }
  }

  mrd->loop();
  check_status();
}
