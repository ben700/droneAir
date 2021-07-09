#ifndef DroneGoogle_h
#define DroneGoogle_h
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include <WiFiClientSecureBearSSL.h>
#include <time.h>
#include <MQTT.h>
#include <CloudIoTCore.h>
#include <CloudIoTCoreMqtt.h>


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
      String data = sensors->deviceStatePayload();
      Serial.print(F("Sending Droneponics state data to goolge ... "));
      Serial.println(data);
      Serial.println(mqtt->publishState(data.substring(0,482)) ? "Success!" : "Failed!");
      delay(5);
      Serial.println(publishTelemetry(String(stateTopic), data ) ? "Success!" : "Failed!");

}
void processBoot()
{

  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }

  Serial.print(F("Sending Droneponics boot data to goolge ... "));
  //Serial.println(publishTelemetry(String(deviceBootTopic), String(sensors->bootPayload(String(timeClient.getEpochTime())))) ? "Success!" : "Failed!");
  Serial.println(publishTelemetry(String(onlineStateTopic), "{\"eventType\":\"CONNECT\"}" ) ? "Success!" : "Failed!");
  


}

void processSensor (){
  if(!sensors->loggingData) { return;}
  
   while (!timeClient.update()) {
      timeClient.forceUpdate();
   }
 
    String payload = sensors->sensorPayload(String(timeClient.getEpochTime()));
    bool returnCode = mqtt->publishTelemetry(sensorReadingTopic, payload);
  
    Serial.print(F("Sending telemetry sensor topic: "));
    Serial.println(returnCode ? "Success!" : "Failed!"); 

    if(!returnCode){
      Serial.print(F("Payload was : "));
      Serial.println(payload);
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
    bool return_code = sensors->processCommand(doc);
    if(return_code) processState();
  }

  if(String(topic).indexOf("config") > 0){
    bool return_code = sensors->processConfig(doc);
    if(return_code) processSensor();
  }
  
}

static void readDerCert(const char *filename) {
  File ca = LittleFS.open(filename, "r");
  if (ca)
  {
    size_t size = ca.size();
    uint8_t cert[size];
    ca.read(cert, size);
    certList.append(cert, size);
    ca.close();

    Serial.print("Success to open ca file ");
  }
  else
  {
    Serial.print("Failed to open ca file ");
  }
  Serial.println(filename);
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

  readDerCert("/data/gtsltsr.crt"); // primary_ca.pem
  readDerCert("/data/GSR4.crt"); // backup_ca.pem
  netClient.setTrustAnchors(&certList);


  File f = LittleFS.open("/data/private-key.der", "r");
  if (f) {
    size_t size = f.size();
    uint8_t data[size];
    f.read(data, size);
    f.close();

    BearSSL::PrivateKey pk(data, size);
    device.setPrivateKey(pk.getEC()->x);

    Serial.println("Success to open private-key.der");
  } else {
    Serial.println("Failed to open private-key.der");
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
