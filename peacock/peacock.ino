#include <HeadingCMPS11.h>
#include <TidyGPS.h>
#include <Beat.h>

#include <SoftwareSerial.h>

static const int BEAT_PIN = 9;

static const double TARGET_LAT = 50.853683 , TARGET_LON = -0.117813;
static const int TARGET_RANGE_TIGHT = 5; // margin of error for within tight range of target
static const int TARGET_RANGE_LOOSE = 45; // margin of error for within loose range of target

SoftwareSerial ss_gps1 = SoftwareSerial(3, 4);
TidyGPS gps(&ss_gps1);

SoftwareSerial ss_cmps11 = SoftwareSerial(6, 5);
HeadingCMPS11 cmps11(&ss_cmps11);
unsigned int current_angle;

enum beatType {
  NONE,
  STEADY,
  RAPID
};

beatType currentBeat = NONE;
Beat beat(BEAT_PIN);

void setup()
{
  Serial.begin(115200);
  gps.begin(9600);
  gps.setTargetCoords(TARGET_LAT, TARGET_LON );
  cmps11.begin(9600);

}

void loop()
{
  gps.advance();
  beat.advance();
  current_angle = cmps11.getHeading();

  calculateAngle();
  //doBeat();
}


void calculateAngle()
{
  if ( gps.isBearingWithinTolerance( current_angle, 5 ))
  {
 
    if ( currentBeat != RAPID )
    {
      currentBeat = RAPID;
      beat.stop();
      beat.start(10, 100, 10, 200);
    }

  }

 else if ( gps.isBearingWithinTolerance( current_angle, 45 ))
  {
    if ( currentBeat != STEADY)
    {
      currentBeat = STEADY;
      beat.stop();
      beat.start(20, 200, 20, 2000);
    }

  }

  else if (  currentBeat != NONE )
  {
    Serial.println("NONE");
    currentBeat = NONE;
    beat.stop();
  }
}

void doBeat()
{

}



