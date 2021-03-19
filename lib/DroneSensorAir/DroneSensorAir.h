#ifndef DroneSensorAir_h
#define DroneSensorAir_h

#include "Arduino.h"

#include "Wire.h"

#define DOC_SIZE 1000
#include <stdint.h>
#include <ArduinoJson.h>
#include <BlueDot_BME280_TSL2591.h>


class DroneSensorAir {
 public:
    DroneSensorAir (String __deviceMAC, String __deviceIP, String __deviceID, bool _DroneSensor_debug);
    bool processCommand(StaticJsonDocument<DOC_SIZE>& _command);
    bool processConfig(StaticJsonDocument<DOC_SIZE>& _config);
    String sensorPayload(String _EpochTime);
    String bootPayload(String _EpochTime);
    String deviceStatePayload ();
    void setFallbackTemp(float __FallbackTemp);

    int pollDelay = 60000;
    bool loggingData = true;
    bool DroneSensor_debug=false;
   
  private:
    String _deviceMAC;
    String _deviceIP;
    String _deviceID;
    float _FallbackTemp = 19.5;
    BlueDot_BME280_TSL2591 bme280;
    BlueDot_BME280_TSL2591 tsl2591;

};

#endif
