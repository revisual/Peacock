#include <HeadingCMPS11.h>
#include <TidyGPS.h>

#include <SoftwareSerial.h>


//SoftwareSerial gps = SoftwareSerial(3, 4);


static const int BEATING_DELAYS[] = {10, 150, 10, 1500};
static const int PANIC_DELAYS[] = {10, 10, 10, 10};

static int solenoidPin = 9;

static const double TARGET_LAT = 50.853683 , TARGET_LON = -0.117813;
static const int TARGET_RANGE_TIGHT = 5; // margin of error for within tight range of target
static const int TARGET_RANGE_LOOSE = 45; // margin of error for within loose range of target

SoftwareSerial ss_gps1 = SoftwareSerial(3, 4);
TidyGPS gps(&ss_gps1);

SoftwareSerial ss_cmps11 = SoftwareSerial(6, 5);
HeadingCMPS11 cmps11(&ss_cmps11);
unsigned int current_angle;

void setup()
{
  Serial.begin(115200);
  gps.begin(9600);
  gps.setTargetCoords(TARGET_LAT,TARGET_LON );
  cmps11.begin(9600);
  pinMode(solenoidPin, OUTPUT);
}

void loop()
{
  gps.advance();
  doCompass();

  

  calculateAngle();
  //doBeat();
}


void doCompass()
{
  unsigned int new_angle;

  new_angle = cmps11.getHeading();

  if ( new_angle != current_angle)
  {
    current_angle = new_angle;
    Serial.print(current_angle, DEC);
    Serial.println();
  }

}

void calculateAngle()
{
 if ( gps.isBearingWithinTolerance( current_angle, 5 ))
  {
    digitalWrite(solenoidPin, HIGH);
    Serial.println("HIT");
  }

  else if ( gps.isBearingWithinTolerance( current_angle, 45 ))
  {
    Serial.println("WARM");
  }
  else
  {
    digitalWrite(solenoidPin, LOW);
    Serial.println("MISS");
  }
}

void doBeat()
{

}



