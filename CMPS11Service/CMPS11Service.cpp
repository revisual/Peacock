/*
  
*/

#include "Arduino.h"
#include "CMPS11Service.h"
#include "SoftwareSerial.h"

const int CMPS11Service::CMPS_GET_ANGLE16 = 0x13;

CMPS11Service::CMPS11Service(SoftwareSerial *ss)
{
  _cmps11 = ss;  
}

void CMPS11Service::begin(long speed)
{
  _cmps11->begin(9600); 
}

unsigned int CMPS11Service::getHeading()
{
  unsigned char high_byte;
  unsigned char low_byte;
  unsigned int angle16;
  _cmps11->listen();
  _cmps11->write(CMPS_GET_ANGLE16);  // Request and read 16 bit angle
  while (_cmps11->available() < 2);
  high_byte = _cmps11->read();
  low_byte = _cmps11->read();
  angle16 = high_byte;                // Calculate 16 bit angle
  angle16 <<= 8;
  angle16 += low_byte;

  return angle16 / 10; 
}
