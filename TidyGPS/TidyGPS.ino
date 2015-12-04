#include <SoftwareSerial.h>
#include <TinyGPS.h>

TinyGPS _tinyGPS;
SoftwareSerial _gps = SoftwareSerial(3, 4);

enum GPS_STATE {
  RESET,
  WRITING,
  READING,
  RESTING
};

GPS_STATE _gpsState = RESET;
unsigned long _timerStart;
unsigned long _readCycle = 500;
unsigned long _restCycle = 5000;

int _currentBearing = 0;
unsigned int _currentDistance = 100;

double _targetLat = 0;
double _targetLon = 0;

float _currentLat = 0;
float _currentLon = 0;
unsigned long _ageOfFix = 10000;

void setup()
{
  Serial.begin(115200);
  begin(9600);
  setTargetCoords(50.819276, -0.135355);
}

void loop()
{


}

void setReadingCycle(unsigned long reading, unsigned long resting )
{
  _readCycle = reading;
  _restCycle = resting;
}

void setTargetCoords(double lat, double lon)
{
  _targetLat = lat;
  _targetLon = lon;
}

int getCurrentBearing()
{
  return _currentBearing;
}

unsigned int  getCurrentDistance()
{
  return _currentDistance;
}

unsigned long  getAgeOfFix()
{
  return _ageOfFix;
}

void begin(long speed)
{
  _gps.begin(speed);
  gpsRead(1000);
}

void advance()
{
  _gps.listen();

  applyPosition();

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


  // another way is to a track the number of valid sentences and abort read after attaining
  // gpsRead(0);

}

void applyPosition()
{
  _tinyGPS.f_get_position(&_currentLat, &_currentLon, &_ageOfFix);
}

void gpsWrite()
{

  _currentBearing = (int)_tinyGPS.course_to(_currentLat, _currentLon, _targetLat, _targetLon);
  _currentDistance = (unsigned int )_tinyGPS.distance_between(_currentLat, _currentLon, _targetLat, _targetLon) / 1000;

  //print_float(flat, TinyGPS::GPS_INVALID_F_ANGLE, 10, 6);
  //Serial.print( String(_ageOfFix));
  //print_int(_ageOfFix, TinyGPS::GPS_INVALID_AGE, 5);
  //print_float(flon, TinyGPS::GPS_INVALID_F_ANGLE, 11, 6);
  //print_float(flat == TinyGPS::GPS_INVALID_F_ANGLE ? TinyGPS::GPS_INVALID_F_ANGLE : TinyGPS::course_to(flat, flon, _targetLat, _targetLon), TinyGPS::GPS_INVALID_F_ANGLE, 7, 2);
  //Serial.println(" _currentBearing = " + (String)_currentBearing);
  //Serial.println();
  //Serial.println( "_currentBearing = " + _currentBearing );

}


bool isDistanceWithinTolerance( unsigned int tolerance )
{
  return ( _currentDistance < tolerance );
}


bool isBearingWithinTolerance( unsigned int heading, unsigned int tolerance )
{
  // clamp tolerance to 45;
  if ( tolerance > 45 )
  {
    Serial.print("clamping heading tolerance to 45");
    tolerance = 45;
  }

  // adjust for compass wrap around by moving angles into the low end of the circle;
  if ( _currentBearing < tolerance || _currentBearing > 360 - tolerance)
  {
    int gps_heading_adjusted = (_currentBearing + (tolerance * 2)) % 360;
    int heading_adjusted = (heading + (tolerance * 2)) % 360;
    return testHeading( gps_heading_adjusted, heading_adjusted, tolerance);
  }

  else
  {
    return testHeading( _currentBearing, heading, tolerance);
  }

}

bool testHeading( unsigned int gpsheading, unsigned int heading, unsigned int tolerance )
{
  int headingsDiff = abs(gpsheading - heading);
  return (headingsDiff <= tolerance) ;
}

void gpsRead(unsigned long ms)
{
  bool r = false;
  unsigned long start = millis();
  do
  {
    while (_gps.available())
    {
      _tinyGPS.encode(_gps.read());
    }

  } while (millis() - start < ms);

}




