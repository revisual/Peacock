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

    static const byte RESET = 0;
    static const byte WRITING = 1;
    static const byte READING = 2;
    static const byte RESTING = 3;

    static const byte INVALID = 4;
    static const byte FRESH = 5;
    static const byte STALE = 6;    
    
    byte _gpsState = RESET;
   
    byte currentFreshness = INVALID;

    unsigned long _timerStart = 0;
    unsigned long _readCycleFresh = 111;
    unsigned long _restCycleFresh = 1000;
    unsigned long _readCycleStale = 333;
    unsigned long _restCycleStale = 1000;
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
