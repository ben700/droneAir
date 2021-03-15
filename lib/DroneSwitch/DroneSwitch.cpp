#include "DroneSwitch.h"
  
DroneSwitch::DroneSwitch()
{
  DroneSwitch(DroneSwitch_i2c_address, DroneSwitch_StartRelay);
}

DroneSwitch::DroneSwitch(byte i2c, int StartRelay)
{
  this->_i2c = i2c;
  this->_size = 16 - StartRelay;
  Wire.begin();

  Wire.beginTransmission(this->_i2c);
  Wire.write(0x00);            // A register
  Wire.write(0x00);            // set all of port A to outputs
  Wire.endTransmission();

  Wire.beginTransmission(this->_i2c);
  Wire.write(0x01);            // B register
  Wire.write(0x00);            // set all of port B to outputs
  Wire.endTransmission();       
  turnAllOff();
  _doseQTicker.attach_ms(100, std::bind(&DroneSwitch::processDoseQ, this));
}

void DroneSwitch::turnOff(int num)
{
  num--;
  bitWrite(this->_i2c_register, _pins[num],0);
  doCommmit();
  _tickers[num].detach();
  _doseQTicker.attach_ms(100, std::bind(&DroneSwitch::processDoseQ, this));
}
void DroneSwitch::turnOn(int num)
{
  num--;
  bitWrite(this->_i2c_register, _pins[num],1);
  doCommmit();
}
void DroneSwitch::turnAllOff()
{
  this->_i2c_register = 0b0000000000000000;
  doCommmit();    
}
void DroneSwitch::turnAllOn()
{
  this->_i2c_register = 0b1111111111111111;
  doCommmit();    
}
int DroneSwitch::get(int num)
{
  num--;
  return bitRead(this->_i2c_register, _pins[num]);
}
void DroneSwitch::set(int num, bool status)
{
   num--;
   (status) ? turnOn(num): turnOff(num);
}

int DroneSwitch::getSize(){
  return this->_size;
}

String DroneSwitch::getName(int num){
  num--;
  return "Relay_" + String(num+1);
}

StaticJsonDocument<1000> DroneSwitch::payload()
{
  StaticJsonDocument<1000> _doc;
  for(int i =0; i <this->_size; i++){
    if(bitRead(this->_i2c_register, _pins[i]) == 1){
	    _doc["Relay_" + String(i+1)]["Status"] ="On";
    }else{
	    _doc["Relay_" + String(i+1)]["Status"] = "Off";	    
    }
    _doc["Relay_" + String(i+1)]["Name"] = doser_list[i]._name;
    _doc["Relay_" + String(i+1)]["Volume"] = String(doser_list[i]._volume);
  }
  return _doc;
}
void DroneSwitch::dose(int num, int volume){
  turnOn(num);
  _tickers[num-1].attach_ms(volume, std::bind(&DroneSwitch::turnOff, this, num));
}

int DroneSwitch::getDoseQSize(){
  return  _doseQ.getCount();
}

void DroneSwitch::clearDoseQ(){
    _doseQ.flush();
}
void DroneSwitch::addDose(int num, int volume){
  //DroneQItem item = DroneQItem(num,volume) ;
  QItem qItem = {num, volume};
  
  bool duplicate = false;
	for (int j = (int) _doseQ.getCount() ; j >= 0 ; j--)
  {
			QItem chk;
			_doseQ.peekIdx(&chk, j);
			if (!memcmp(&qItem, &chk, sizeof(QItem)))
			{
				duplicate = true;
				break;
			}
		}
  if (!duplicate)	{ _doseQ.push(&qItem); }
}

void DroneSwitch::processDoseQ(){
   if (_doseQ.isEmpty()){ return; }
   _doseQTicker.detach();
   QItem qItem;
   _doseQ.pop(&qItem);
   dose(qItem._relay, qItem._volume);
}   

void DroneSwitch::doCommmit()
{
  unsigned char variable_LOW = lowByte(this->_i2c_register);
  unsigned char variable_HIGH = highByte(this->_i2c_register);
  Wire.beginTransmission(this->_i2c);
  Wire.write(0x12);                     // address bank A
  Wire.write(variable_LOW);
  Wire.endTransmission();
     
  Wire.beginTransmission(this->_i2c);
  Wire.write(0x13);                     // address bank B
  Wire.write(variable_HIGH);
  Wire.endTransmission();
}
