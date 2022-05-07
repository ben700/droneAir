#ifndef DroneDebug_h
#define DroneDebug_h
#include "Arduino.h"

class DroneDebug
{
public:
    DroneDebug(String _prefix);
    ~DroneDebug();

    
    static void DEBUG_MSG(String _prefix, String text);
    


    void DEBUG_WM(String text);
    void setPrefix(String _prefix);

private:
   String prefix;
};

#endif
