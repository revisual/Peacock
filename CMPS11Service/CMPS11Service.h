/*
 
*/
#ifndef CMPS11Service_h
#define CMPS11Service_h

#include "Arduino.h"
#include "SoftwareSerial.h"

class CMPS11Service
{
  public:
    CMPS11Service(SoftwareSerial *ss);
    unsigned int getHeading();
    void begin(long speed);
  private:
    unsigned int _currentHeading;
    SoftwareSerial * _cmps11;
    static const int CMPS_GET_ANGLE16;
};

#endif
