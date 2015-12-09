#include <HeadingCMPS11.h>
#include <TidyGPS.h>
#include <Beat.h>

#include <SoftwareSerial.h>

static const int BEAT_PIN = 9;

static const double TARGET_LAT = 50.842537 , TARGET_LON = -0.137835;
static const int TARGET_RANGE_TIGHT = 5; // margin of error for within tight range of target
static const int TARGET_RANGE_LOOSE = 45; // margin of error for within loose range of target
static const int TARGET_DISTANCE = 5; // margin of error for within loose range of target

static const unsigned long FRESHNESS_TARGET = 10000; // data considered stale if _ageOfFix exceeds this

SoftwareSerial ss_gps1 = SoftwareSerial(3, 4);
TidyGPS gps(&ss_gps1);

SoftwareSerial ss_cmps11 = SoftwareSerial(6, 5);
HeadingCMPS11 cmps11(&ss_cmps11);

Beat beat(BEAT_PIN);
enum BeatType {
  NONE,
  STEADY,
  RAPID,
  CONSTANT
};

BeatType currentBeat = NONE;

enum FreshnessType {
  
  INVALID,
  FRESH,
  STALE
};

FreshnessType currentFreshness = INVALID;

void setup()
{
  Serial.begin(115200);
  gps.begin(9600);
  gps.setReadingCycle(100, 1000);
  gps.setTargetCoords(TARGET_LAT, TARGET_LON );
  cmps11.begin(9600);
}

void loop()
{
  gps.advance();
  beat.advance();
  setBearing(cmps11.getHeading());
  trackFreshness();
}

void trackFreshness()
{
  bool isFresh =  gps.isFresherThan(FRESHNESS_TARGET);

  Serial.println("fresh = " + (String)isFresh);

  if ( !gps.isValid() && currentFreshness != INVALID)
  {
   // beat.start(100, 100, 100, 100);
    currentFreshness = INVALID;
    return;
  }

  else if (isFresh && currentFreshness != FRESH)
  {
    currentFreshness = FRESH;
    gps.setReadingCycle(100, 1000);
    Serial.println("adjusting for freshness (100)");
    //Serial.println("hdop = "+(String)gps.getHDOP());
  }

  else if (!isFresh && currentFreshness != STALE)
  {
    currentFreshness = STALE;
    gps.setReadingCycle(333, 1000);
    Serial.println("adjusting for staleness (333)");
    //Serial.println("fresh = "+(String)gps.isFresherThan(10000));
    //Serial.println("hdop = "+(String)gps.getHDOP());
  }
}

void setBearing(unsigned int current_angle)
{
  if( !gps.isValid()) return;
  
  if ( gps.isDistanceWithinTolerance( TARGET_DISTANCE ))
  {
    if ( currentBeat != CONSTANT )
    {
      currentBeat = CONSTANT;
      //beat.stop();
      //beat.on();
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



