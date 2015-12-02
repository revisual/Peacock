#include <SoftwareSerial.h>
#include <TinyGPS.h>

TinyGPS tiny_gps;
SoftwareSerial gps = SoftwareSerial(3,4);

static void gps_smartdelay();
static void print_float(float val, float invalid, int len, int prec);
static void print_int(unsigned long val, unsigned long invalid, int len);
static void print_date(TinyGPS &tiny_gps);
static void print_str(const char *str, int len);


#define CMPS_GET_ANGLE8 0x12
#define CMPS_GET_ANGLE16 0x13


enum GPS_STATE {
  NONE,
  WRITING,
  READING 
};

static GPS_STATE gps_state = NONE;

SoftwareSerial cmps11 = SoftwareSerial(6,5);
unsigned char high_byte, low_byte, angle8;
unsigned int angle16;
unsigned int current_angle;

void setup()
{
  Serial.begin(115200);  
  gps.begin(9600);
  cmps11.begin(9600);
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

  if( gps_state == NONE)
  {
      gps_state = READING;
  }

  if( gps_state == READING)
  {
      gps_smartdelay(); 
  }

  else if (gps_state == WRITING)
  {     
    getGPSData();
    gps_smartdelay(); 
    gps_state = NONE;
  }
  
}

void getGPSData()
{
   float flat, flon;
  unsigned long age, date, time, chars = 0;
  unsigned short sentences = 0, failed = 0;
  static const double TARGET_LAT = 51.508131, TARGET_LON = -0.128002;
   tiny_gps.f_get_position(&flat, &flon, &age);
   Serial.print("GPS::");
   Serial.println();
   print_str(tiny_gps.f_course() == TinyGPS::GPS_INVALID_F_ANGLE ? "*** " : TinyGPS::cardinal(tiny_gps.f_course()), 6);
   print_int(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0xFFFFFFFF : (unsigned long)TinyGPS::distance_between(flat, flon, TARGET_LAT, TARGET_LON) / 1000, 0xFFFFFFFF, 9);
   print_float(flat == TinyGPS::GPS_INVALID_F_ANGLE ? TinyGPS::GPS_INVALID_F_ANGLE : TinyGPS::course_to(flat, flon, TARGET_LAT, TARGET_LON), TinyGPS::GPS_INVALID_F_ANGLE, 7, 2);
   print_str(flat == TinyGPS::GPS_INVALID_F_ANGLE ? "*** " : TinyGPS::cardinal(TinyGPS::course_to(flat, flon, TARGET_LAT, TARGET_LON)), 6);
 
   tiny_gps.stats(&chars, &sentences, &failed);
   print_int(chars, 0xFFFFFFFF, 6);
  
   Serial.println();
}

void doCompass()
{
  unsigned int new_angle;
  cmps11.listen();
  cmps11.write(CMPS_GET_ANGLE16);  // Request and read 16 bit angle
  while(cmps11.available() < 2);
  high_byte = cmps11.read();
  low_byte = cmps11.read();
  angle16 = high_byte;                // Calculate 16 bit angle
  angle16 <<= 8;
  angle16 += low_byte;  

  new_angle = angle16 / 10;

  if( new_angle != current_angle)
  {
    current_angle = new_angle;
    Serial.print(" cmps11   angle full: ");       // Display 16 bit angle with decimal place
  
    Serial.print(current_angle, DEC);
    Serial.print(".");
    Serial.print(angle16 % 10, DEC);

    Serial.println();
  }

 
}

void calculateAngle()
{
  
}

void doBeat()
{
  
}

static void gps_smartdelay()
{ 
  bool finished = false;
 
  while (gps.available())
  {
     tiny_gps.encode(gps.read());
     finished = true;
  }

  if (finished)
  {
    gps_state = WRITING;
  }     
 
}


static void print_float(float val, float invalid, int len, int prec)
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
    for (int i=flen; i<len; ++i)
      Serial.print(' ');
  }
  //gps_smartdelay();
}

static void print_int(unsigned long val, unsigned long invalid, int len)
{
  char sz[32];
  if (val == invalid)
    strcpy(sz, "*******");
  else
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0) 
    sz[len-1] = ' ';
  Serial.print(sz);
 // gps_smartdelay();
}

static void print_date(TinyGPS &tiny_gps)
{
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned long age;
  tiny_gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  if (age == TinyGPS::GPS_INVALID_AGE)
    Serial.print("********** ******** ");
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d %02d:%02d:%02d ",
        month, day, year, hour, minute, second);
    Serial.print(sz);
  }
  print_int(age, TinyGPS::GPS_INVALID_AGE, 5);
  //gps_smartdelay();
}

static void print_str(const char *str, int len)
{
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
    Serial.print(i<slen ? str[i] : ' ');
  //gps_smartdelay();
}
