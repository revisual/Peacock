/*
 
*/
#ifndef HeadingCMPS11_h
#define HeadingCMPS11_h

#include "Arduino.h"
#include "SoftwareSerial.h"

class HeadingCMPS11
{
  public:
    HeadingCMPS11(SoftwareSerial *ss);
    unsigned int getHeading();
    void begin(long speed);
  private:
    unsigned int _currentHeading;
    SoftwareSerial * _cmps11;
    static const int CMPS_GET_ANGLE16;
};

#endif