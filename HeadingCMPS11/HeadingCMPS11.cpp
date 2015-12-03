/*
  
*/

#include "Arduino.h"
#include "HeadingCMPS11.h"
#include "SoftwareSerial.h"

const int HeadingCMPS11::CMPS_GET_ANGLE16 = 0x13;

HeadingCMPS11::HeadingCMPS11(SoftwareSerial *ss)
{
  _cmps11 = ss;  
}

void HeadingCMPS11::begin(long speed)
{
  _cmps11->begin(9600); 
}

unsigned int HeadingCMPS11::getHeading()
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
