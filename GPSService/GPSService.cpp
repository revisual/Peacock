
#include "Arduino.h"
#include "GPSService.h"
#include "TinyGPS.h"
#include "SoftwareSerial.h"


GPSService::GPSService(SoftwareSerial *ss)
{
  _gps = ss;
}

void GPSService::begin(long speed)
{
  _gps->begin(speed);
}

void GPSService::run()
{
  _gps->listen();

  applyPosition();
  applyStats();

  if ( _gpsState == RESET)
  {
    _timerStart = millis();
    _gpsState = READING;
  }

  if ( _gpsState == READING)
  {
    gpsRead(_readCycle);
    _gpsState = WRITING;
  }

  else if (_gpsState == WRITING)
  {
    gpsWrite();
    _timerStart = millis();
    _gpsState = RESTING;
  }

  else if (_gpsState == RESTING)
  {
    if (millis() - _timerStart > _restCycle)
    {
      _gpsState = RESET;
    }
  }

  trackFreshness();
}

void GPSService::setFreshnessTolerance(unsigned long ms)
{
  _freshnessTolerance = ms;
}

void GPSService::setFreshReadingCycle(unsigned long reading, unsigned long resting )
{
  _readCycleFresh = reading;
  _restCycleFresh = resting;
}

void GPSService::setStaleReadingCycle(unsigned long reading, unsigned long resting )
{
  _readCycleStale = reading;
  _restCycleStale = resting;
}

void GPSService::setTargetCoords(double lat, double lon)
{
  _targetLat = lat;
  _targetLon = lon;
}

int GPSService::getCurrentBearing()
{
  return _currentBearing;
}

unsigned int  GPSService::getCurrentDistance()
{
  return _currentDistance;
}

unsigned long  GPSService::getAgeOfFix()
{
  return _ageOfFix;
}

unsigned long GPSService::getHDOP()
{
  return _tinyGPS.hdop();
}

bool GPSService::isValid()
{
  return (_ageOfFix != TinyGPS::GPS_INVALID_AGE);
}

bool GPSService::isFresherThan( unsigned long ms)
{
  return (_ageOfFix < ms);
}

bool GPSService::isDistanceWithinTolerance( unsigned int tolerance )
{
  return ( _currentDistance < tolerance );
}

bool GPSService::isBearingWithinTolerance( unsigned int heading, unsigned int tolerance )
{
  if ( tolerance > 45 )
  {
    Serial.print("clamping heading tolerance to 45");
    tolerance = 45;
  }

  // adjust for compass wrap around by moving angles into the low end of the circle;
  if ( _currentBearing < tolerance || _currentBearing > 360 - tolerance)
  {
    int gpsHeadingAdjusted = (_currentBearing + (tolerance * 2)) % 360;
    int headingAdjusted = (heading + (tolerance * 2)) % 360;
    return testHeading( gpsHeadingAdjusted, headingAdjusted, tolerance);
  }

  else
  {
    return testHeading( _currentBearing, heading, tolerance);
  }
}

void GPSService::applyPosition()
{
  _tinyGPS.f_get_position(&_currentLat, &_currentLon, &_ageOfFix);
}

void GPSService::applyStats()
{
  _tinyGPS.stats(&_chars, &_sentences, &_failed);
}

void GPSService::gpsRead(unsigned long ms)
{
  unsigned long start = millis();
  bool hasRead;
  unsigned short prevSentences = _sentences;
  do
  {
    hasRead = false;
    while (_gps->available())
    {
      _tinyGPS.encode(_gps->read());
      hasRead = true;
    }

    if ( hasRead && isMoreValidThan( prevSentences ))
    {
      return;
    }

  } while (millis() - start < ms);
}

bool GPSService::isMoreValidThan(unsigned short value)
{
  applyStats();
  return ( _sentences > value );
}

void GPSService::gpsWrite()
{
  _currentBearing = (int)_tinyGPS.course_to(_currentLat, _currentLon, _targetLat, _targetLon);
  _currentDistance = (unsigned int )_tinyGPS.distance_between(_currentLat, _currentLon, _targetLat, _targetLon);
}

bool GPSService::testHeading( unsigned int gpsBearing, unsigned int bearing, unsigned int tolerance )
{
  int diff = abs(gpsBearing - bearing);
  return (diff <= tolerance) ;
}

void GPSService::trackFreshness()
{
  bool isFresh =  isFresherThan(_freshnessTolerance);

  if ( !isValid() )
  {
    if ( currentFreshness != INVALID)
    {
      currentFreshness = INVALID;
    }
    return;
  }

  else if (isFresh && currentFreshness != FRESH)
  {
    currentFreshness = FRESH;
    _readCycle = _readCycleFresh;
    _restCycle = _restCycleFresh;
  }

  else if (!isFresh && currentFreshness != STALE)
  {
    currentFreshness = STALE;
    _readCycle = _readCycleStale;
    _restCycle = _restCycleStale;
  }
}


