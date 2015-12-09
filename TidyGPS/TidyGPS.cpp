/*

*/

#include "Arduino.h"
#include "TidyGPS.h"
#include "TinyGPS.h"
#include "SoftwareSerial.h"


TidyGPS::TidyGPS(SoftwareSerial *ss)
{
  _gps = ss;
}

void TidyGPS::begin(long speed)
{
  _gps->begin(speed);
}

void TidyGPS::advance()
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
}

void TidyGPS::setReadingCycle(unsigned long reading, unsigned long resting )
{
  _readCycle = reading;
  _restCycle = resting;
}

void TidyGPS::setTargetCoords(double lat, double lon)
{
  _targetLat = lat;
  _targetLon = lon;
}

int TidyGPS::getCurrentBearing()
{
  return _currentBearing;
}

unsigned int  TidyGPS::getCurrentDistance()
{
  return _currentDistance;
}

unsigned long  TidyGPS::getAgeOfFix()
{
  return _ageOfFix;
}

unsigned long TidyGPS::getHDOP()
{
  return _tinyGPS.hdop();
}

bool TidyGPS::isValid()
{
  return (_ageOfFix != TinyGPS::GPS_INVALID_AGE);
}

bool TidyGPS::isFresherThan( unsigned long ms)
{
  return (_ageOfFix < ms);
}

bool TidyGPS::isDistanceWithinTolerance( unsigned int tolerance )
{
  return ( _currentDistance < tolerance );
}

bool TidyGPS::isBearingWithinTolerance( unsigned int heading, unsigned int tolerance )
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

void TidyGPS::applyPosition()
{
  _tinyGPS.f_get_position(&_currentLat, &_currentLon, &_ageOfFix);
}

void TidyGPS::applyStats()
{
  _tinyGPS.stats(&_chars, &_sentences, &_failed);
}

void TidyGPS::gpsRead(unsigned long ms)
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

bool TidyGPS::isMoreValidThan(unsigned short value)
{
  applyStats();
  return ( _sentences > value );
}

void TidyGPS::gpsWrite()
{
  _currentBearing = (int)_tinyGPS.course_to(_currentLat, _currentLon, _targetLat, _targetLon);
  _currentDistance = (unsigned int )_tinyGPS.distance_between(_currentLat, _currentLon, _targetLat, _targetLon);
}

bool TidyGPS::testHeading( unsigned int gpsBearing, unsigned int bearing, unsigned int tolerance )
{
  int diff = abs(gpsBearing - bearing);
  return (diff <= tolerance) ;
}


