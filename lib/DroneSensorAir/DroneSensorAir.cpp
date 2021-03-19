#include "DroneSensorAir.h"

DroneSensorAir::DroneSensorAir(String __deviceMAC, String __deviceIP, String __deviceID, bool _DroneSensor_debug = false)
{
  this->bme280.parameter.I2CAddress = 0x77;                 //The BME280 is hardwired to use the I2C Address 0x77              
  this->tsl2591.parameter.I2CAddress = 0x29; 
  DroneSensor_debug = _DroneSensor_debug;
  this->_deviceMAC = __deviceMAC;
  this->_deviceIP = __deviceIP;
  this->_deviceID = __deviceID;
  Wire.begin(); 
  this->tsl2591.parameter.gain = 0b01;
  this->tsl2591.parameter.integration = 0b000; 
  this->tsl2591.config_TSL2591();
  this->bme280.parameter.sensorMode = 0b11; 
  this->bme280.parameter.IIRfilter = 0b100; 
  this->bme280.parameter.humidOversampling = 0b101;
  this->bme280.parameter.tempOversampling = 0b101;
  this->bme280.parameter.pressOversampling = 0b101; 
  this->bme280.parameter.pressureSeaLevel = 1013.25;
  this->bme280.parameter.tempOutsideCelsius = this->_FallbackTemp;   
}

String DroneSensorAir::sensorPayload(String _EpochTime)
{
  if (DroneSensor_debug) { Serial.println(F("DroneSensor::sensorPayload()"));  }
  StaticJsonDocument<DOC_SIZE> doc;
     
  doc["deviceTime"] = _EpochTime;
  doc["deviceMAC"] = this->_deviceMAC;

   doc["temperature"] = this->bme280.readTempC(); 
   doc["humidity"] = this->bme280.readHumidity();
   doc["pressure"] = this->bme280.readPressure();
   doc["altitude"] = this->bme280.readAltitudeMeter();
   doc["lux"] = this->tsl2591.getFullLuminosity_TSL2591();
   doc["infrared"] = this->tsl2591.getInfrared_TSL2591();
   doc["visible"] = this->tsl2591.getVisibleLight_TSL2591();
   doc["full_spectrum"] = this->tsl2591.getVisibleLight_TSL2591();
   
   
  if(DroneSensor_debug ){serializeJsonPretty(doc, Serial);}
  String output;
  serializeJson(doc, output);
  return output; 
}

void DroneSensorAir::setFallbackTemp(float __FallbackTemp){
  this->_FallbackTemp = __FallbackTemp;
  this->bme280.parameter.tempOutsideCelsius = this->_FallbackTemp;   
}

String DroneSensorAir::deviceStatePayload (){
  StaticJsonDocument<DOC_SIZE> doc;
 // doc["Fallback Temperature"] = this->_FallbackTemp;
  doc["Fallback Temp"] = this->_FallbackTemp;
  doc["Poll Delay"] = this->pollDelay;
  if(DroneSensor_debug){serializeJsonPretty(doc, Serial);Serial.println("");}
  String output;
  serializeJson(doc, output);
  return output; 
}

bool DroneSensorAir::processConfig(StaticJsonDocument<DOC_SIZE>& _config){
  bool returnCode = true;
  if(_config["Fallback Temperature"] != NULL){
    float __fallbackTemperature = _config["Fallback Temperature"].as<float>();
    if (DroneSensor_debug) {Serial.println("Setting Fallback Temperature to " + String(__fallbackTemperature));}
    if(__fallbackTemperature > 0){
      setFallbackTemp(__fallbackTemperature);
    }
  }

  if(_config["Poll Delay"] != NULL){
    int __pollDelay = _config["Poll Delay"].as<int>();
    if (DroneSensor_debug) {Serial.println("Setting Poll Delay to " + String(__pollDelay));}
    if(__pollDelay > 100){
      this->pollDelay =__pollDelay;
    }
  }  

  if(_config["logData"] != NULL){
    if(_config["logData"] == "No"){
      if (DroneSensor_debug) {Serial.println(F("Turning Off logging data"));}
      this->loggingData = false;
    }else{
      if (DroneSensor_debug) {Serial.println(F("Turning on logging data"));}
      this->loggingData = true;
    }
  }
  return returnCode;
}

bool DroneSensorAir::processCommand(StaticJsonDocument<DOC_SIZE>& _command){

  return true;
}
  

String DroneSensorAir::bootPayload(String _EpochTime){
  if (DroneSensor_debug) { Serial.println(F("DroneSensor::bootPayload()")); Serial.println(""); }
  StaticJsonDocument<DOC_SIZE> doc;
     
  doc["bootTime"] = _EpochTime;
  doc["deviceMAC"] = this->_deviceMAC;
  doc["deviceName"] = String(this->_deviceID);
  doc["deviceIP"] = String(this->_deviceIP);

  if(DroneSensor_debug){serializeJsonPretty(doc, Serial);Serial.println("");}
    
  String payload;
  serializeJson(doc, payload);
  return payload;
}