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

static const byte BEAT_PIN = 5;

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

SoftwareSerial ss_gps1 = SoftwareSerial(6, 7);
GPSService gps(&ss_gps1);

SoftwareSerial ss_cmps11 = SoftwareSerial(9, 8);
CMPS11Service cmps11(&ss_cmps11);

WayPoint _waypoints;
SimpleTimer _timer;
Beat beat(BEAT_PIN);
FSM _fsm;
SerialIn _serialIn;

byte readingState = ACTION_NONE;
String inputString = "";

void setup()
{
  Serial.begin(9600);

  gps.begin(9600);
  gps.setFreshnessTolerance(10000);
  gps.setFreshReadingCycle(111, 1000);
  gps.setStaleReadingCycle(333, 1000);

  cmps11.begin(9600);

  beat.configureState(STEADY_HEART, 20, 200, 20, 1666);
  beat.configureState(RAPID_HEART, 18, 188, 18, 444);
  beat.configureState(CONSTANT_REGULAR_SLOW, 20, 1666, 20, 1666);

  _fsm.setEnterStateCallbacks(REQUEST_DATA, enterRequestData);
  _fsm.setEnterStateCallbacks(CHECKING_SYSTEM, enterCheckingSystem);
  _fsm.setLoopStateCallbacks(SYSTEM_READY, loopSystemReady);
  _fsm.setEnterStateCallbacks(NAVIGATING_TO_WAYPOINT, enterNavigatingToWayPoint);
   _fsm.setLoopStateCallbacks(NAVIGATING_TO_WAYPOINT, loopNavigatingToWayPoint);
  _fsm.setEnterStateCallbacks(ARRIVING_AT_WAYPOINT, enterArrivingAtWayPoint);
  _fsm.setEnterStateCallbacks(COMPLETED, enterArrivingAtWayPoint);


  _serialIn.setCallbacks('|', onSerialDataStart, onSerialDataEnd);


  _fsm.changeState(REQUEST_DATA);

}

void loop()
{
  gps.advance();
  beat.advance();
  _timer.run();
  _fsm.run();
}

void enterRequestData()
{
  Serial.print(F("#data$"));
  _timer.setTimeout(500, checkDataReceived);
}

void checkDataReceived()
{
 // Serial.print(F("checkDataReceived"));
  if ( _fsm.getCurrentState() != REQUEST_DATA) return;
  enterRequestData();
}

void enterCheckingSystem()
{
  Serial.print(F("@test.wav$"));
  beat.setState(CONSTANT_REGULAR_SLOW);
  _fsm.changeState(SYSTEM_READY);
  Serial.print(_fsm.getCurrentState());
}

void loopSystemReady()
{
  Serial.print(F("READY"));
  if (  !gps.isValid())return; 
  _fsm.changeState(NAVIGATING_TO_WAYPOINT);
}

void enterNavigatingToWayPoint()
{
   Serial.print(F("naving"));
    beat.setState(NONE);
  setBearing(cmps11.getHeading());
  checkDistance();
}

void loopNavigatingToWayPoint()
{
  setBearing(cmps11.getHeading());
  checkDistance();
}

void enterArrivingAtWayPoint()
{
  _waypoints.next();
  applyWayPoint();
  beat.setState(CONSTANT_REGULAR_SLOW);
  _timer.setTimeout(5000, changeToNavigating);
}

void enterCompleted()
{
  beat.setState(CONSTANT_REGULAR_SLOW);
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
  //Serial.println("DATA START :: " + input);
  WayPointData data(inputString, ',');
  _fsm.changeState(READING);
  _waypoints.add( data );
}

void onSerialDataEnd( String input)
{
  //Serial.println("DATA END :: " + input);
  applyWayPoint(  );
  _fsm.changeState(CHECKING_SYSTEM);
  _waypoints.reset();
}

void checkDistance()
{
  if ( _waypoints.isComplete() )
  {
    _fsm.changeState(COMPLETED);
  }

  else if ( _waypoints.hasArrived(gps.getCurrentDistance()))
  {
    _fsm.changeState(ARRIVING_AT_WAYPOINT);
  }
}

void applyWayPoint(  )
{
  gps.setTargetCoords(_waypoints.currentLat(), _waypoints.currentLon() );
  Serial.print( _waypoints.currentAction() + END_CHAR);
}

void setBearing(unsigned int current_angle)
{

  if ( gps.isBearingWithinTolerance( current_angle, TARGET_RANGE_TIGHT ))
  {
    beat.setState(RAPID_HEART);
  }

  else if ( gps.isBearingWithinTolerance( current_angle, TARGET_RANGE_LOOSE ))
  {
    beat.setState(STEADY_HEART);
  }

  else
  {
    beat.setState(NONE);
  }
}
