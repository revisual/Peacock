#include <CMPS11Service.h>
#include <GPSService.h>
#include <WayPoint.h>
#include <Beat.h>
#include <FSM.h>
#include <SimpleTimer.h>
#include <SerialIn.h>

#include <SoftwareSerial.h>

/*
   example data:
   |A,50.846357,-0.133361,5,@a.wav|B,50.846426,-0.132604,5,@b.wav|C,50.847461,-0.132846,5,@c.wav|D,50.845832,-0.13328,5,@d.wav|$
    |A,50.846356940159865,-0.13340473215066595,5,@a.wav|B,50.8464416175486,-0.13261079828225775,5,@b.wav|C,50.84742725285132,-0.1328843832743587,5,@c.wav|D,50.84585225875958,-0.13329207937204046,5,@d.wav|$
*/

static const byte BEAT_PIN = 9;
static const byte RED_PIN = 8;
static const byte GREEN_PIN = 7;

static const int TARGET_RANGE_TIGHT = 5; // mar0gin of error for within tight range of target
static const int TARGET_RANGE_LOOSE = 45; // margin of error for within loose range of target

static const char LOG_CHAR = '~';
static const char PLAY_CHAR = '@';
static const char PIPE_CHAR = '|';
static const char END_CHAR = '$';

static const byte ACTION_NONE = 0;
static const byte ACTION_LOG = 1;
static const byte ACTION_PLAY = 2;
static const byte ACTION_DESERIALISE = 3;

static const byte REQUEST_DATA = 1;
static const byte READING = 2;
static const byte CHECKING_SYSTEM = 3;
static const byte SYSTEM_READY = 4;
static const byte NAVIGATING_TO_WAYPOINT = 5;
static const byte ARRIVING_AT_WAYPOINT = 6;
static const byte COMPLETED = 7;

static const byte NONE = 0;
static const byte STEADY_HEART = 1;
static const byte RAPID_HEART = 2;
static const byte CONSTANT_REGULAR_SLOW = 3;

SoftwareSerial ss_gps = SoftwareSerial(4,3);
GPSService _gps(&ss_gps);

SoftwareSerial ss_cmps11 = SoftwareSerial(9, 8);
CMPS11Service _cmps(&ss_cmps11);

WayPoint _waypoints;
SimpleTimer _timer;
Beat _beat(BEAT_PIN);
FSM _fsm;
SerialIn _serialIn;

void setup()
{
  Serial.begin(9600);

  _gps.begin(9600);
  _gps.setFreshnessTolerance(10000);
  _gps.setFreshReadingCycle(111, 1000);
  _gps.setStaleReadingCycle(333, 1000);

  _cmps.begin(9600);

  _beat.configureState(STEADY_HEART, 20, 200, 20, 1666);
  _beat.configureState(RAPID_HEART, 18, 188, 18, 444);
  _beat.configureState(CONSTANT_REGULAR_SLOW, 20, 1666, 20, 1666);

  _fsm.setEnterCallback(REQUEST_DATA, enterRequestData);
  _fsm.setEnterCallback(CHECKING_SYSTEM, enterCheckingSystem);
  _fsm.setRunCallback(SYSTEM_READY, loopSystemReady);
  _fsm.setEnterCallback(NAVIGATING_TO_WAYPOINT, enterNavigatingToWayPoint);
  _fsm.setRunCallback(NAVIGATING_TO_WAYPOINT, loopNavigatingToWayPoint);
  _fsm.setEnterCallback(ARRIVING_AT_WAYPOINT, enterArrivingAtWayPoint);
  _fsm.setEnterCallback(COMPLETED, enterArrivingAtWayPoint);
  _fsm.changeState(REQUEST_DATA);
  // _fsm.changeState(CHECKING_SYSTEM);

  _serialIn.setCallbacks(PIPE_CHAR, onSerialDataStart, onSerialDataEnd);

  _timer.setInterval(10000, logPosition);

}

void logPosition()
{
  if ( !_gps.isValid())
  {
    log(F("----------------"));
    log(F("NOT VALID:"));

  }
  else
  {
    log(F("----------------"));
    log(F("LAT:"));
    log((String)_gps.getCurrentLat());
    log(F("LONG:"));
    log((String)_gps.getCurrentLong());
    log(F("BEARING:"));
    log((String)_gps.getCurrentBearing());
    log(F("DISTANCE:"));
    log((String)_gps.getCurrentDistance());
    log(F("AGE:"));
    log((String)_gps.getAgeOfFix());
  }


}

void loop()
{
  _gps.run();
  _beat.run();
  _timer.run();
  _fsm.run();

  digitalWrite(GREEN_PIN, (_gps.isValid()) ? HIGH : LOW);
  digitalWrite(RED_PIN, (_gps.isValid() || _gps.isFresherThan(10000)) ? LOW : HIGH);

}

void enterRequestData()
{
  if ( _fsm.getCurrentState() != REQUEST_DATA) return;
  Serial.print(F("#data.txt$"));
  _timer.setTimeout(500, checkDataReceived);
}

void checkDataReceived()
{
  if ( _fsm.getCurrentState() != REQUEST_DATA) return;
  enterRequestData();
}

void enterCheckingSystem()
{
  digitalWrite(RED_PIN, HIGH );
  Serial.println(F("@startup.wav$"));
  _beat.setState(CONSTANT_REGULAR_SLOW);
  _fsm.changeState(SYSTEM_READY);
}

void loopSystemReady()
{
  if (!_gps.isValid())return;
  Serial.println(F("@valid.wav$"));
  _fsm.changeState(NAVIGATING_TO_WAYPOINT);
}

void enterNavigatingToWayPoint()
{
  log(F("M:s5"));
  _beat.setState(NONE);
}

void loopNavigatingToWayPoint()
{
  setBearing(_cmps.getHeading());
  checkDistance();
}

void enterArrivingAtWayPoint()
{
  log(F("M:s6"));
  Serial.println( _waypoints.currentAction() + END_CHAR);
  _waypoints.next();
  applyWayPoint();
  _beat.setState(CONSTANT_REGULAR_SLOW);
  _timer.setTimeout(5000, changeToNavigating);
}

void enterCompleted()
{
  _beat.setState(CONSTANT_REGULAR_SLOW);
}

void changeToNavigating()
{
  _fsm.changeState(NAVIGATING_TO_WAYPOINT);
}

void serialEvent()
{
  _serialIn.run();
}

void onSerialDataStart( String input)
{

  WayPointData data(input, ',');

  if (_waypoints.add( data ))
  {
    //Serial.println("DATA:: " + input);
    _fsm.changeState(READING);
  }

  /*else
    {
    Serial.println(F("ERROR ADDING DATA"));
    }*/

}

void onSerialDataEnd( String input)
{
  _waypoints.reset();
  applyWayPoint(  );
  _fsm.changeState(CHECKING_SYSTEM);
}

void checkDistance()
{
  if ( _waypoints.isComplete() )
  {
    _fsm.changeState(COMPLETED);
  }

  else if ( _waypoints.hasArrived(_gps.getCurrentDistance()))
  {
    _fsm.changeState(ARRIVING_AT_WAYPOINT);
  }
}

void applyWayPoint(  )
{
  _gps.setTargetCoords(_waypoints.currentLat(), _waypoints.currentLon() );
  //Serial.println( "Lat: " + (String)_waypoints.currentLat());
  //Serial.println( "Lon: " + (String)_waypoints.currentLon());
}

void setBearing(unsigned int current_angle)
{

  if ( _gps.isBearingWithinTolerance( current_angle, TARGET_RANGE_TIGHT ))
  {
    _beat.setState(RAPID_HEART);
  }

  else if ( _gps.isBearingWithinTolerance( current_angle, TARGET_RANGE_LOOSE ))
  {
    _beat.setState(STEADY_HEART);
  }

  else
  {
    _beat.setState(NONE);
  }
}

void log( String msg)
{
  Serial.print(LOG_CHAR + msg + END_CHAR);
}

