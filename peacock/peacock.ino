#include <CMPS11Service.h>
#include <GPSService.h>
#include <WayPoint.h>
#include <Beat.h>
#include <SimpleTimer.h>

#include <SoftwareSerial.h>

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

static const byte STARTING_UP = 0;
static const byte READING = 1;
static const byte CHECKING_SYSTEM = 2;
static const byte SYSTEM_READY = 3;
static const byte NAVIGATING_TO_WAYPOINT = 4;
static const byte ARRIVING_AT_WAYPOINT = 5;
static const byte RESTING_AT_WAYPOINT = 6;
static const byte FINALISING = 7;
static const byte COMPLETED = 8;

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

byte readingState = ACTION_NONE;
byte currentState = READING;
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
}

void loop()
{
  gps.advance();
  beat.advance();
  _timer.run();
  runState();
}

void runState()
{
  if ( currentState == READING )
  {
    //nothing - waiting for serial data;
  }

  else if ( currentState == CHECKING_SYSTEM )
  {
    beat.setState(CONSTANT_REGULAR_SLOW);
    currentState = SYSTEM_READY;
  }

  else if ( currentState == SYSTEM_READY)
  {
    if (  gps.isValid())
    {
      changeToNavigating();
    }
    //nothing - waiting for changeToNavigating timeout;
  }

  else if ( currentState == NAVIGATING_TO_WAYPOINT )
  {
    setBearing(cmps11.getHeading());
    checkDistance();
  }

  else if ( currentState == ARRIVING_AT_WAYPOINT)
  {
    _waypoints.next();
    applyWayPoint();
    beat.setState(CONSTANT_REGULAR_SLOW);
    currentState = RESTING_AT_WAYPOINT;
    _timer.setTimeout(5000, changeToNavigating);
  }

  else if ( currentState == RESTING_AT_WAYPOINT)
  {
    // nothing - waiting for changeToNavigating timeout;
  }

  else if (currentState == FINALISING)
  {
    beat.setState(CONSTANT_REGULAR_SLOW);
    currentState = COMPLETED;
  }
}

void serialEvent() 
{
  if (readingState == ACTION_NONE)
  {
    findNextSpecial();
  }

  else  if (readingState == ACTION_DESERIALISE)
  {
    readData();
  }
}

void findNextSpecial()
{
  while (Serial.available()) {

    char inChar = (char)Serial.read();

    if (inChar == PIPE_CHAR)
    {
      readData();
    }
  }
}

void readData()
{
  if ( currentState != READING ) return;
  
  readingState == ACTION_DESERIALISE;
  
  while (Serial.available()) {

    char inChar = (char)Serial.read();

    if (inChar == PIPE_CHAR)
    {
      Serial.println(inputString );
      WayPointData data(inputString, ',');
      _waypoints.add( data );
      inputString = "";
    }

    else if (inChar == END_CHAR)
    {
      applyWayPoint(  );
      currentState = CHECKING_SYSTEM;
      _waypoints.reset();
      readingState = ACTION_NONE;
      inputString = "";
      break;
    }

    else
    {
      inputString += inChar;
    }
  }
}

void changeToNavigating()
{
  currentState = NAVIGATING_TO_WAYPOINT;
  beat.setState(NONE);
}

void checkDistance()
{
  if ( _waypoints.isComplete() )
  {
    currentState = FINALISING;
  }

  else if ( _waypoints.hasArrived(gps.getCurrentDistance()))
  {
    currentState = ARRIVING_AT_WAYPOINT;
  }
}

void applyWayPoint(  )
{
  gps.setTargetCoords(_waypoints.currentLat(), _waypoints.currentLon() );
  Serial.print(PLAY_CHAR + _waypoints.currentAction() + END_CHAR);
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
