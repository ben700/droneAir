#include "droneDebug.h"

DroneDebug::DroneDebug(String _prefix)
{
    setPrefix(_prefix);
}


 void DroneDebug::setPrefix(String _prefix)
{
    this->prefix = "*" + _prefix + ": ";
}

template <typename Generic>
void DroneDebug::DEBUG_MSG(String _prefix, Generic text)
{
    String __prefix = "*" + _prefix + ": ";
    if (DEBUG)
    {

        Serial.println(__prefix + text);
    }
}

template <typename Generic>
void DroneDebug::DEBUG_WM(Generic text)
{
    if (DEBUG)
    {

        Serial.println(this->prefix + text);
    }
}