#include <HeadingCMPS11.h>

#include <SoftwareSerial.h>
#include <TinyGPS.h>

TinyGPS tiny_gps;
SoftwareSerial gps = SoftwareSerial(3, 4);

static void gps_smartdelay(unsigned long ms);

static const int BEATING_DELAYS[] = {10, 150, 10, 1500};
static const int PANIC_DELAYS[] = {10, 10, 10, 10};

enum HEART_STATE {
  RESTING,
  BEATING,
  PANIC
};

static HEART_STATE heart_state = RESTING;

enum GPS_STATE {
  RESET,
  WRITING,
  READING
};

static GPS_STATE gps_state = RESET;
static unsigned long gps_start;
static const unsigned long gps_delay = 1000;
static int solenoidPin = 9;

static int gps_heading = 0;

static const double TARGET_LAT = 51.508131, TARGET_LON = -0.128002;
static const int TARGET_RANGE_TIGHT = 5; // margin of error for within tight range of target
static const int TARGET_RANGE_LOOSE = 45; // margin of error for within loose range of target

SoftwareSerial ss_cmps11 = SoftwareSerial(6, 5);
HeadingCMPS11 cmps11(&ss_cmps11);
unsigned int current_angle;

void setup()
{
  Serial.begin(115200);
  gps.begin(9600);
  cmps11.begin(9600);
  pinMode(solenoidPin, OUTPUT);
}

void loop()
{
  doGPS();
  doCompass();
  calculateAngle();
  doBeat();
}

void doGPS()
{
  gps.listen();

  if ( gps_state == RESET)
  {
    gps_start = millis();
    gps_state = READING;
  }

  if ( gps_state == READING)
  {
    gps_smartdelay(gps_delay);
  }

  else if (gps_state == WRITING)
  {
    applyGPSData();
    gps_state = RESET;
  }

}

void applyGPSData()
{
  float flat, flon;
  unsigned long age;

  tiny_gps.f_get_position(&flat, &flon, &age);
  gps_heading = 180;//(int)tiny_gps.course_to(flat, flon, TARGET_LAT, TARGET_LON);
  Serial.print("gps_heading = " + gps_heading);
  Serial.println();


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
  int headingsDiff = abs(gps_heading - current_angle);

  if (headingsDiff <= TARGET_RANGE_TIGHT)
  {
    //if your heading is close enough to the target heading, then make something happen
    if ( heart_state != PANIC )
    {
      Serial.println("THAT WAY!");
      heart_state = PANIC;
    }

  }

  else  if (headingsDiff <= TARGET_RANGE_LOOSE)
  {
    if ( heart_state != BEATING )
    {
      Serial.println("WARM");
      heart_state = BEATING;
    }

  }

  else if (headingsDiff > TARGET_RANGE_LOOSE)
  {
    if ( heart_state != RESTING )

    { // you're still off target

      Serial.println("NOTHING");
      heart_state = RESTING;
    }
  }
}

void doBeat()
{
  if ( heart_state == PANIC )
  {

  }

  else if ( heart_state == BEATING )
  {

  }

  else if ( heart_state == RESTING )
  {

  }
}

static void gps_smartdelay(unsigned long ms)
{
  while (gps.available())
  {
    tiny_gps.encode(gps.read());
  }

  if ( (millis() - gps_start) > gps_delay)
  {
    gps_state = WRITING;
  }

}



