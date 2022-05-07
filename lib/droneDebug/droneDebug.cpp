#include "droneDebug.h"

DroneDebug::DroneDebug(String _prefix)
{
    setPrefix(_prefix);
}


 void DroneDebug::setPrefix(String _prefix)
{
    this->prefix = "*" + _prefix + ": ";
}

void DroneDebug::DEBUG_MSG(String _prefix, String text)
{
    if (DEBUG)
    {

        Serial.println("*" + _prefix + ": " + text);
    }
}

void DroneDebug::DEBUG_WM(String text)
{
    if (DEBUG)
    {

        Serial.println(this->prefix + text);
    }
}