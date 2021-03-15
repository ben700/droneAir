#ifndef DroneSwitch_h_
#define DroneSwitch_h_

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>
#include <ArduinoJson.h>
#include <Ticker.h>
#include <cppQueue.h>


#define DroneSwitch_i2c_address 0x27   // i2c slave address for chip expander
#define DroneSwitch_StartRelay 8   // i2c address for relay, thqy use 0-15 for 16 way and 8-15 for 8 way and 13-15 on 4 way

typedef struct Item {
  int _relay;
  int _volume;
} QItem;

typedef struct Switch {
  int _pin;
  String _name;
  int _volume;
} Doser;

class DroneSwitch
{
  public:
    DroneSwitch();
    DroneSwitch(byte i2c, int StartRelay);  
    void turnOn(int num);
    void turnOff(int num);
    void turnAllOn();
    void turnAllOff();
    String getName(int num);
    StaticJsonDocument<1000>  payload();
    int getSize();
    int get(int num);
    void set(int num, bool status);
    void dose(int num, int volume);
    void addDose(int num, int volume);
    int getDoseQSize();
    void clearDoseQ();
    Doser pump1 = {15, "Pump 1" ,1000};
    Doser pump2 = {14, "Pump 2" ,1000};
    Doser pump3 = {13, "Pump 3" ,1000};
    Doser pump4 = {12, "Pump 4" ,1000};
    Doser pump5 = {11, "Pump 5" ,1000};
    Doser pump6 = {10, "Pump 6" ,1000};
    Doser pump7 = {9, "Pump 7" ,1000};
    Doser pump8 = {8, "Pump 8" ,1000};
    Doser pump9 = {7, "Pump 9" ,1000};
    Doser pump10 = {6, "Pump 10" ,1000};
    Doser pump11 = {5, "Pump 11" ,1000};
    Doser pump12 = {4, "Pump 12" ,1000};
    Doser pump13 = {3, "Pump 13" ,1000};
    Doser pump14 = {2, "Pump 14" ,1000};
    Doser pump15 = {1, "Pump 15" ,1000};
    Doser pump16 = {0, "Pump 16" ,1000};
  
    //array of ezo boards, add any new boards in here for the commands to work with them
    Doser doser_list[16] = {
      pump1,
      pump2,
      pump3,
      pump4,
      pump5,
      pump6,
      pump7,
      pump8,
      pump9,
      pump10,
      pump11,
      pump12,
      pump13,
      pump14,
      pump15,
      pump16
     };
  private:
    void doCommmit();
    void processDoseQ();
    byte _i2c;
    int _pins[16]= {8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7};


  
    unsigned int _i2c_register = 0b1111111111111111;
    int _size = 16;
    cppQueue	_doseQ = cppQueue(sizeof(QItem), 16, FIFO);
    Ticker _doseQTicker;
    
    Ticker relay1Ticker;
    Ticker relay2Ticker;
    Ticker relay3Ticker;
    Ticker relay4Ticker;
    Ticker relay5Ticker;
    Ticker relay6Ticker;
    Ticker relay7Ticker;
    Ticker relay8Ticker;
    Ticker relay9Ticker;
    Ticker relay10Ticker;
    Ticker relay11Ticker;
    Ticker relay12Ticker;
    Ticker relay13Ticker;
    Ticker relay14Ticker;
    Ticker relay15Ticker;
    Ticker relay16Ticker;
    Ticker _tickers[16] = {relay1Ticker, relay2Ticker, relay3Ticker, relay4Ticker, relay5Ticker, relay6Ticker, relay7Ticker, relay8Ticker, relay9Ticker, relay10Ticker, relay11Ticker, relay12Ticker, relay13Ticker, relay14Ticker, relay15Ticker, relay16Ticker};
};
#endif
