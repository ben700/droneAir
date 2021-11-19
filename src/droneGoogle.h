#ifndef DroneGoogle_h
#define DroneGoogle_h
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include <WiFiClientSecureBearSSL.h>
#include <time.h>
#include <MQTT.h>
#include <CloudIoTCore.h>
#include <CloudIoTCoreMqtt.h>
#define DOC_SIZE 1000

#include "droneConfig.h"
#include <ArduinoJson.h>


// Initialize WiFi and MQTT for this board
static MQTTClient *mqttClient;
static BearSSL::WiFiClientSecure netClient;
static BearSSL::X509List certList;
static CloudIoTCoreDevice device(project_id, location, registry_id, device_id);
CloudIoTCoreMqtt *mqtt;

bool publishTelemetry(String data)
{
  return mqtt->publishTelemetry(data);
}

bool publishTelemetry(const char *data, int length)
{
  return mqtt->publishTelemetry(data, length);
}

bool publishTelemetry(String subfolder, String data)
{
  return mqtt->publishTelemetry(subfolder, data);
}

bool publishTelemetry(String subfolder, const char *data, int length)
{
  return mqtt->publishTelemetry(subfolder, data, length);
}

 
String getJwt()
{
  // Disable software watchdog as these operations can take a while.
  ESP.wdtDisable();
  time_t iat = time(nullptr);
  Serial.println(F("Refreshing JWT"));
  String jwt = device.createJWT(iat, jwt_exp_secs);
  ESP.wdtEnable(0);
  return jwt;
}


void processState()
{

  

      StaticJsonDocument<DOC_SIZE> doc;
      doc["Middleware"] = CURRENT_VERSION;
   
      serializeJsonPretty(doc, Serial);
      
      String data;
      serializeJson(doc, data);
 
      Serial.print(F("Sending Droneponics state data to goolge ... "));
      Serial.println(data);
      Serial.println(mqtt->publishState(data) ? "Success!" : "Failed!");
 
}

void processSensor (){
  
   while (!timeClient.update()) {
      timeClient.forceUpdate();
   }

      StaticJsonDocument<DOC_SIZE> doc;
      doc["deviceTime"] = String(timeClient.getEpochTime());
      doc["deviceMAC"] = WiFi.macAddress();
  
     Serial.print(F("Duration in Seconds:\t\t"));
   Serial.println(float(millis())/1000);
 
   Serial.print(F("Temperature in Celsius:\t\t")); 
   Serial.println(bme280.readTempC());
    doc["temperature"] = bme280.readTempC();

   Serial.print(F("Humidity in %:\t\t\t")); 
   Serial.println(bme280.readHumidity());
doc["humidity"] = bme280.readHumidity();

   Serial.print(F("Pressure in hPa:\t\t")); 
   Serial.println(bme280.readPressure());
doc["pressure"] = bme280.readPressure();

   Serial.print(F("Altitude in Meters:\t\t")); 
   Serial.println(bme280.readAltitudeMeter());
doc["altitude"] = bme280.readAltitudeMeter();

   Serial.print(F("Illuminance in Lux:\t\t")); 
   Serial.println(tsl2591.readIlluminance_TSL2591());
doc["lux"] = tsl2591.readIlluminance_TSL2591();

 
   
      serializeJsonPretty(doc, Serial);
      
      String payload;
      serializeJson(doc, payload);
 
    bool returnCode = mqtt->publishTelemetry(sensorReadingTopic, payload);
    Serial.print(F("Sending telemetry sensor topic: "));
    Serial.println(returnCode ? "Success!" : "Failed!"); 

    if(!returnCode){
      Serial.print(F("Payload was : "));
   //   Serial.println(payload);
    }

}

void messageReceivedAdvanced(MQTTClient *client, char topic[], char bytes[], int length){
  if (length > 0){
    Serial.printf("incoming: %s - %s\n", topic, bytes);
  } else {
    Serial.printf("0\n"); // Success but no message
  }
  
  StaticJsonDocument<1000> doc;
  DeserializationError error = deserializeJson(doc, bytes);
  if (error)
     Serial.println(F("Failed to read file, using default configuration"));

  if(String(topic).indexOf("command") > 0){
  //  bool return_code = sensors->processCommand(doc);
  //  if(return_code) processState();
  }

  if(String(topic).indexOf("config") > 0){
 //   bool return_code = sensors->processConfig(doc);
 //   if(return_code) processSensor();
  }
  
}
void littleFsListDir(String dirname)
{

  Serial.println("Listing directory : " + dirname);
  Dir root = LittleFS.openDir(dirname);

  while (root.next())
  {
    File file = root.openFile("r");
    Serial.print("  File: ");
    Serial.println(root.fileName());
    file.close();
  }
}

static void readDerCert(String filename)
{
  File ca = LittleFS.open(filename, "r");
  if (ca)
  {
    size_t size = ca.size();
    uint8_t cert[size];
    ca.read(cert, size);
    certList.append(cert, size);
    ca.close();

    Serial.print("Success to open ca file ");
    Serial.println(filename);
  }
  else
  {
    Serial.print("Failed to open ca file:-  ");
    Serial.println(filename);
    Serial.println("Failed to open ca file. Security dir contents are :");
    littleFsListDir(CAPath);
  }
}
static void setupCertAndPrivateKey()
{

  // If using the (preferred) method with the cert and private key in /data (SPIFFS)
  // To get the private key run
  // openssl ec -in <private-key.pem> -outform DER -out private-key.der

  if (!LittleFS.begin())
  {
    Serial.println("Failed to mount file system");
    return;
  }
   netClient.setBufferSizes(1024, 1024);

  readDerCert(primaryCA); // primary_ca.pem
  readDerCert(backupCA);  // backup_ca.pem
  netClient.setTrustAnchors(&certList);

  File f = LittleFS.open(privateKeyPath, "r");

  if (f)
  {
    size_t size = f.size();
    uint8_t data[size];
    f.read(data, size);
    f.close();

    BearSSL::PrivateKey pk(data, size);
    device.setPrivateKey(pk.getEC()->x);

    Serial.println("Success to open " + privateKeyPath);
  }
  else
  {
    Serial.println("Failed to open " + privateKeyPath);
    Serial.print(" Contents of ID dir are:-");
    littleFsListDir(idPath);
  }
  LittleFS.end();
}

static void setupWifi()
{
  configTime(0, 0, ntp_primary, ntp_secondary);
  Serial.println("Waiting on time sync...");
  while (time(nullptr) < 1510644967)
  {
    delay(10);
  }
}


// TODO: fix globals
void setupCloudIoT()
{
  // ESP8266 WiFi setup
  setupWifi();

  // ESP8266 WiFi secure initialization and device private key
  setupCertAndPrivateKey();

  mqttClient = new MQTTClient(512);
  mqttClient->setOptions(180, true, 1000); // keepAlive, cleanSession, timeout
  mqtt = new CloudIoTCoreMqtt(mqttClient, &netClient, &device);
  mqtt->setUseLts(true);
  mqtt->startMQTTAdvanced(); // Opens connection using advanced callback
}
#endif
