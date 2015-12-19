#include <CMPS11Service.h>
#include <GPSService.h>
#include <Beat.h>

#include <SoftwareSerial.h>

static const int BEAT_PIN = 9;

static const double TARGET_LAT = 50.842537 , TARGET_LON = -0.137835;
static const int TARGET_RANGE_TIGHT = 5; // margin of error for within tight range of target
static const int TARGET_RANGE_LOOSE = 45; // margin of error for within loose range of target
static const int TARGET_DISTANCE = 5; // margin of error for within loose range of target

SoftwareSerial ss_gps1 = SoftwareSerial(6, 7);
GPSService gps(&ss_gps1);

SoftwareSerial ss_cmps11 = SoftwareSerial(8, 9);
CMPS11Service cmps11(&ss_cmps11);

Beat beat(BEAT_PIN);

enum State {
  STARTINGUP, //on start up
  READING,    //immediatly reads and deserialises data
  CHECKING_SYSTEM, // test solonid, comms out / in, music
  NAVIGATING_TO_WAYPOINT, //walking towards next waypoint
  RESTING_AT_WAYPOINT,    //waypoint attained next waypoint retreieved, music plays etc
  COMPLETE    //reached final destination
};

State currentState = STARTINGUP;

void setup()
{
  Serial.begin(115200);

  gps.begin(9600);
  gps.setFreshnessTolerance(10000);
  gps.setFreshReadingCycle(111, 1000);
  gps.setStaleReadingCycle(333, 1000);
  gps.setTargetCoords(TARGET_LAT, TARGET_LON );

  cmps11.begin(9600);

  beat.configureState(beat.STEADY_HEART, 20, 200, 20, 1500);
  beat.configureState(beat.RAPID_HEART, 15, 150, 15, 500);
}

void loop()
{
  gps.advance();
  beat.advance();
  //setBearing(cmps11.getHeading());
  Serial.println("age of fix = " + (String)gps.getAgeOfFix());
  Serial.println("heading = " + (String)cmps11.getHeading());
}

void setBearing(unsigned int current_angle)
{
  if ( !gps.isValid()) return;

  if ( gps.isDistanceWithinTolerance( TARGET_DISTANCE ))
  {

  }

  else if ( gps.isBearingWithinTolerance( current_angle, TARGET_RANGE_TIGHT ))
  {
    beat.setState(beat.RAPID_HEART);
  }

  else if ( gps.isBearingWithinTolerance( current_angle, TARGET_RANGE_LOOSE ))
  {
    beat.setState(beat.STEADY_HEART);
  }

  else
  {
    beat.setState(beat.NONE);
  }
}
