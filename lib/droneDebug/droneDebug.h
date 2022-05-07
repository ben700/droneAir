#ifndef DroneDebug_h
#define DroneDebug_h
#include "Arduino.h"

class DroneDebug
{
public:
    DroneDebug(String _prefix);
    ~DroneDebug();

    template <typename Generic>
    static void DEBUG_MSG(String _prefix, Generic text);
    

    template <typename Generic>
    void DEBUG_WM(Generic text);
    void setPrefix(String _prefix);

private:
   String prefix;
};

#endif
