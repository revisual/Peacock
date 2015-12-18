#include <CMPS11Service.h>
#include <GPSService.h>
#include <Beat.h>

#include <SoftwareSerial.h>

static const int BEAT_PIN = 9;

static const double TARGET_LAT = 50.842537 , TARGET_LON = -0.137835;
static const int TARGET_RANGE_TIGHT = 5; // margin of error for within tight range of target
static const int TARGET_RANGE_LOOSE = 45; // margin of error for within loose range of target
static const int TARGET_DISTANCE = 5; // margin of error for within loose range of target

SoftwareSerial ss_gps1 = SoftwareSerial(3, 4);
GPSService gps(&ss_gps1);

SoftwareSerial ss_cmps11 = SoftwareSerial(6, 5);
CMPS11Service cmps11(&ss_cmps11);

Beat beat(BEAT_PIN);
enum BeatType {
  NONE,
  STEADY,
  RAPID,
  CONSTANT
};

BeatType currentBeat = NONE;

void setup()
{
  Serial.begin(115200);
  gps.begin(9600);
  gps.setFreshnessTolerance(10000);
  gps.setFreshReadingCycle(111, 1000);
  gps.setStaleReadingCycle(333, 1000);
  gps.setTargetCoords(TARGET_LAT, TARGET_LON );
  cmps11.begin(9600);
}

void loop()
{
  gps.advance();
  beat.advance();
  setBearing(cmps11.getHeading());
}

void setBearing(unsigned int current_angle)
{
  if ( !gps.isValid()) return;

  if ( gps.isDistanceWithinTolerance( TARGET_DISTANCE ))
  {
    if ( currentBeat != CONSTANT )
    {
      currentBeat = CONSTANT;
    }
  }

  else if ( gps.isBearingWithinTolerance( current_angle, TARGET_RANGE_TIGHT ))
  {
    if ( currentBeat != RAPID )
    {
      currentBeat = RAPID;
      beat.stop();
      beat.start(15, 150, 15, 500);
    }

  }

  else if ( gps.isBearingWithinTolerance( current_angle, TARGET_RANGE_LOOSE ))
  {
    if ( currentBeat != STEADY)    {
      currentBeat = STEADY;
      beat.stop();
      beat.start(20, 200, 20, 1500);
    }
  }

  else if (  currentBeat != NONE )
  {
    currentBeat = NONE;
    beat.stop();
  }
}
