/*

*/
#ifndef GPSService_h
#define GPSService_h

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "TinyGPS.h"

class GPSService
{
  public:
    GPSService(SoftwareSerial *ss);
    void begin(long speed);
    void advance();
    void setFreshnessTolerance(unsigned long ms);
    void setFreshReadingCycle(unsigned long reading, unsigned long resting );
    void setStaleReadingCycle(unsigned long reading, unsigned long resting );
    void setTargetCoords(double lat, double lon);
    int getCurrentBearing();
    unsigned int  getCurrentDistance();
    unsigned long  getAgeOfFix();
    bool isDistanceWithinTolerance( unsigned int tolerance );
    bool isBearingWithinTolerance( unsigned int heading, unsigned int tolerance );
    bool isValid();
    bool isFresherThan( unsigned long ms);
    unsigned long getHDOP();

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

    enum FreshnessType {
      INVALID,
      FRESH,
      STALE
    };
    FreshnessType currentFreshness = INVALID;

    unsigned long _timerStart = 0;
    unsigned long _readCycleFresh = 500;
    unsigned long _restCycleFresh = 5000;
    unsigned long _readCycleStale = 500;
    unsigned long _restCycleStale = 5000;
    unsigned long _readCycle = 500;
    unsigned long _restCycle = 5000;
    unsigned long _freshnessTolerance = 10000;
    int _currentBearing = 0;
    unsigned int _currentDistance = 100;
    double _targetLat = 0;
    double _targetLon = 0;
    float _currentLat = 0;
    float _currentLon = 0;

    unsigned long _ageOfFix = 10000;
    unsigned long _chars = 0;
    unsigned short _sentences = 0;
    unsigned short _failed = 0;

    void applyPosition();
    void applyStats();
    void gpsRead(unsigned long ms);
    void gpsWrite();
    bool testHeading( unsigned int gpsheading, unsigned int heading, unsigned int tolerance );
    bool isMoreValidThan(unsigned short value);
    void trackFreshness();


};

#endif
