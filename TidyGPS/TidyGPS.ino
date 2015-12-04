#include <HeadingCMPS11.h>

#include <SoftwareSerial.h>
#include <TinyGPS.h>

TinyGPS tiny_gps;
SoftwareSerial gps = SoftwareSerial(3, 4);

enum GPS_STATE {
  RESET,
  WRITING,
  READING,
  RESTING
};

GPS_STATE gps_state = RESET;
unsigned long gps_timer_start;
unsigned long gps_read_cycle = 500;
unsigned long gps_rest_cycle = 5000;

int gps_heading = 0;

double target_lat = 0;
double target_lon = 0;
unsigned long age;

SoftwareSerial ss_cmps11 = SoftwareSerial(6, 5);
HeadingCMPS11 cmps11(&ss_cmps11);
unsigned int current_angle;

static int solenoidPin = 9;

void setup()
{
  Serial.begin(115200);
  begin(9600);
  cmps11.begin(9600);

  pinMode(solenoidPin, OUTPUT);
  setCoords(50.819276, -0.135355);


}

void loop()
{
  advance();
  doCompass();
  if ( isHeadingWithinTolerance( current_angle, 5 ))
  {
    digitalWrite(solenoidPin, HIGH);
    Serial.println("HIT");
  }

  // else if ( isHeadingWithinTolerance( current_angle, 45 ))
  //{
  //  Serial.println("WARM");
  //}
  else
  {
    digitalWrite(solenoidPin, LOW);
    // Serial.println("MISS");
  }

}

void setReadingCycle(unsigned long reading, unsigned long resting )
{
  gps_read_cycle = reading;
  gps_rest_cycle = resting;
}

void setCoords(double lat, double lon)
{
  target_lat = lat;
  target_lon = lon;
}

void begin(long speed)
{
  gps.begin(speed);
  gpsRead(1000);
}

void advance()
{
  gps.listen();

  if ( gps_state == RESET)
  {
    gps_timer_start = millis();
    gps_state = READING;
  }

  if ( gps_state == READING)
  {
    gpsRead(gps_read_cycle);
    gps_state = WRITING;
  }

  else if (gps_state == WRITING)
  {
    gpsWrite();
    gps_timer_start = millis();
    gps_state = RESTING;
  }

  else if (gps_state == RESTING)
  {
    if (millis() - gps_timer_start > gps_rest_cycle)
    {
      gps_state = RESET;
    }

  }
  // another way is to a track the number of valid sentences and abort read after attaining
  // gpsRead(0);

}

void gpsWrite()
{
  float flat, flon;


  tiny_gps.f_get_position(&flat, &flon, &age);
  gps_heading = (int)tiny_gps.course_to(flat, flon, target_lat, target_lon);
  print_float(flat, TinyGPS::GPS_INVALID_F_ANGLE, 10, 6);
  Serial.print( String(age));
  //print_int(age, TinyGPS::GPS_INVALID_AGE, 5);
  //print_float(flon, TinyGPS::GPS_INVALID_F_ANGLE, 11, 6);
  //print_float(flat == TinyGPS::GPS_INVALID_F_ANGLE ? TinyGPS::GPS_INVALID_F_ANGLE : TinyGPS::course_to(flat, flon, target_lat, target_lon), TinyGPS::GPS_INVALID_F_ANGLE, 7, 2);
  //Serial.println(" gps_heading = " + (String)gps_heading);
  Serial.println();
  //Serial.println( "gps_heading = " + gps_heading );

}

void print_int(unsigned long val, unsigned long invalid, int len)
{
  char sz[32];
  if (val == invalid)
    strcpy(sz, "*******");
  else
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i = strlen(sz); i < len; ++i)
    sz[i] = ' ';
  if (len > 0)
    sz[len - 1] = ' ';
  Serial.print(sz);
  //smartdelay(0);
}

void print_float(float val, float invalid, int len, int prec)
{
  if (val == invalid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i = flen; i < len; ++i)
      Serial.print(' ');
  }
  // gpsRead();
}


bool isHeadingWithinTolerance( unsigned int heading, unsigned int tolerance )
{
  // clamp tolerance to 90;
  // adjust for compass wrap around;
  if ( gps_heading < tolerance || gps_heading > 360 - tolerance)
  {
    int gps_heading_adjusted = (gps_heading + (tolerance * 2)) % 360;
    int heading_adjusted = (heading + (tolerance * 2)) % 360;
    return testHeading( gps_heading_adjusted, heading_adjusted, tolerance);
  }

  else
  {
    return testHeading( gps_heading, heading, tolerance);
  }

}

bool testHeading( unsigned int gpsheading, unsigned int heading, unsigned int tolerance )
{
  int headingsDiff = abs(gpsheading - heading);
  return (headingsDiff <= tolerance) ;
}


void doCompass()
{
  unsigned int new_angle;

  new_angle = cmps11.getHeading();

  if ( new_angle != current_angle)
  {
    current_angle = new_angle;
    //Serial.prisnt(current_angle, DEC);
    //Serial.println();
  }


}
void gpsRead(unsigned long ms)
{
  bool r = false;
  unsigned long start = millis();
  do
  {
    while (gps.available())
    {
      tiny_gps.encode(gps.read());
    }

  } while (millis() - start < ms);

}


/*void gpsRead()
  {
  while (gps.available())
  {
    Serial.print("@");
    tiny_gps.encode(gps.read());
  }


  if ( (millis() - gps_timer_start) > gps_read_cycle)
  {
    gps_state = WRITING;
  }



  }*/




