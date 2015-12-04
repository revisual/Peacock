/*
 
*/
#ifndef TidyGPS_h
#define TidyGPS_h

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "TinyGPS.h"

class TidyGPS
{
  public:
    TidyGPS(SoftwareSerial *ss);
    void begin(long speed);
    void advance();
    void setReadingCycle(unsigned long reading, unsigned long resting );
    void setTargetCoords(double lat, double lon);
    int getCurrentBearing();
    unsigned int  getCurrentDistance();
    unsigned long  getAgeOfFix();
    bool isDistanceWithinTolerance( unsigned int tolerance );
    bool isBearingWithinTolerance( unsigned int heading, unsigned int tolerance );

  private:
    SoftwareSerial * _gps;
    TinyGPS _tinyGPS;
    enum GPS_STATE {
      RESET,
      WRITING,
      READING,
      RESTING
    };
    GPS_STATE _gpsState = RESET;
    unsigned long _timerStart = 0;
    unsigned long _readCycle = 500;
    unsigned long _restCycle = 5000;
    int _currentBearing = 0;
    unsigned int _currentDistance = 100;
    double _targetLat = 0;
    double _targetLon = 0;
    float _currentLat = 0;
    float _currentLon = 0;
    unsigned long _ageOfFix = 10000;
    void applyPosition();
    void gpsRead(unsigned long ms);
    void gpsWrite();
    bool testHeading( unsigned int gpsheading, unsigned int heading, unsigned int tolerance );
  
};

#endif