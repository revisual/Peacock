/***************************************************************************
   Combined code for Nikki's GPS (Globalsat EM-406a) and compass (HMC5883l)


 ***************************************************************************/
#include <SoftwareSerial.h> //includes the software serial library

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <TinyGPS.h>

TinyGPS gps;
SoftwareSerial gpsmodule(4, 2);

//static const float Place_LAT = 50.934507, Place_LON = -1.435344; //Place where the art is: Southampton
static const float Place_LAT = 50.834091, Place_LON = -0.211455; //Place where the art is: park circle

// assumed starting position
float flat = 50.829605, flon =  -0.210114; // Blast Theory HQ


unsigned long date, time, age; // what they say on the tin
long lat, lon; // +/- lat/long in 100000ths of a degree

unsigned long HBTime = 0;
unsigned long previousHBTime = 0;
//int HBInterval = 5000;

int hbDelay = 500;
long randNumber;

unsigned long pulseTime = 0;
int pulseInterval = 2000;
boolean pulseFlag = 0;



unsigned long GPSInterval = 10000; // check GPS at this interval
unsigned long previousGPSTime = 0; // starting value for this timer variable

int solenoidPin = 3;


/* Assign a unique ID to the compass unit */
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);



// Once you have your heading, you must then add your 'Declination Angle', which is the 'Error' of the magnetic field in your location.
// Find yours here: http://www.magnetic-declination.com/
// Mine is: -13* 2' W, which is ~13 Degrees, or (which we need) 0.22 radians
// If you cannot find your Declination, comment out these two lines, your compass will be slightly off.
float declinationAngle = 0.035;
float headingDegrees = 0;
int heading = 0;
int headings[3];
int avHeading = 0;
int targetHeading = 0;
int headingsDiff = 0;
int targetRangeTight = 5; // margin of error for within tight range of target
int targetRangeLoose = 45; // margin of error for within loose range of target

void displaySensorDetails(void)
{
  sensor_t sensor;
  mag.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       ");
  Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   ");
  Serial.println(sensor.version);
  Serial.print  ("Unique ID:    ");
  Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    ");
  Serial.print(sensor.max_value);
  Serial.println(" uT");
  Serial.print  ("Min Value:    ");
  Serial.print(sensor.min_value);
  Serial.println(" uT");
  Serial.print  ("Resolution:   ");
  Serial.print(sensor.resolution);
  Serial.println(" uT");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void setup(void)
{
  Serial.begin(9600);
  gpsmodule.begin(4800); // serial communication between the gps module and the microcontroller
  int ver = TinyGPS::library_version(); //these 3 lines print out what version of the TinyGPS library is being used
  Serial.print("TinyGPS version: ");
  Serial.println(ver);


  pinMode(solenoidPin, OUTPUT);

  Serial.println("HMC5883 Magnetometer Test");
  Serial.println("");



  /* Initialise the sensor */
  if (!mag.begin())
  {
    /* There was a problem detecting the HMC5883 ... check your connections */
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while (1);
  }

  /* Display some basic information on this sensor */
  displaySensorDetails();

  getGPS(); //Gets GPS data from the module. The sketch won't continue pst here until the GPS module has a fix ?
  Serial.println("wait for signal to stabilise");
  // wait

  //tap solenoid 3 times to say we've got a fix
  digitalWrite(solenoidPin, HIGH);
  delay(1000);
  digitalWrite(solenoidPin, LOW);
  delay(1000);
  digitalWrite(solenoidPin, HIGH);
  delay(1000);
  digitalWrite(solenoidPin, LOW);
  delay(1000);
  digitalWrite(solenoidPin, HIGH);
  delay(1000);
  digitalWrite(solenoidPin, LOW);

  getGPS(); //Gets a fresh lot of data

  Serial.print("start position: ");
  Serial.print(flat, 6);
  Serial.print(", ");
  Serial.println(flon, 6);



  Serial.println(" gps.course_to(flat, flon, Place_LAT, Place_LON)");
  Serial.println(gps.course_to(flat, flon, Place_LAT, Place_LON));
  targetHeading = (int)gps.course_to(flat, flon, Place_LAT, Place_LON);
  Serial.println("targetHeading");
  Serial.println(targetHeading);



  Serial.println("exiting set-up");

}

void loop(void)
{

  //check time since last position
  if (millis() - previousGPSTime > GPSInterval) { //
    previousGPSTime = millis();  // save the current time
    Serial.println("Data from gps:");

    getGPS();


    targetHeading = 180;//(int)gps.course_to(flat, flon, Place_LAT, Place_LON);  //calculate the direction we should be walking in
    Serial.print("updated targetHeading: ");
    Serial.println(targetHeading);
  }

  //else {  //do all the other feedback stuff
    Serial.println("other feedback stuff");




    for (int headingsIndex = 0; headingsIndex < 3; headingsIndex++) {  // we're going to get three values from the compass and average them...
      // this code cycles 3 times to store 3 values in the headings[] array
      getHeading();
      //   Serial.print("Heading (degrees): "); //uncomment line to debug
      //   Serial.println(headingDegrees);  //uncomment line to debig
      heading = (int)headingDegrees;  //changes the value from a float to an integer

      //   Serial.print("Heading (int): ");  //uncomment line to debug
      //  Serial.println(heading);  //uncomment line to debug
      headings[headingsIndex] = heading; //put the integer value into the array

    }

    //now we'll average over the 3 values we've just stored
    // the following chucks out one of the values if it's one side of North compared to the others

    if ((headings[0] > 270 &&  headings[1] < 90 && headings[2] < 90) || (headings[0] < 90 &&  headings[1] > 270 && headings[2] > 270)) {
      avHeading = (headings[1] + headings[2]) / 2;
    }

    else  if ((headings[1] > 270 &&  headings[0] < 90 && headings[2] < 90) || (headings[1] < 90 &&  headings[0] > 270 && headings[2] > 270)) {
      avHeading = (headings[0] + headings[2]) / 2;
    }

    else if ((headings[2] > 270 &&  headings[0] < 90 && headings[1] < 90) || (headings[2] < 90 &&  headings[0] > 270 && headings[1] > 270)) {
      avHeading = (headings[0] + headings[1]) / 2;
    }

    else { // if all are the same side of North just take the mean of all three
      avHeading = (headings[0] + headings[1] + headings[2]) / 3;

    }

    Serial.print("Average heading: ");
    Serial.println(avHeading);


    // compare average heading to direction to chosen place

    headingsDiff = targetHeading - avHeading;
    headingsDiff = abs(headingsDiff);

    Serial.print("targetHeading: ");
    Serial.println(targetHeading);

    Serial.print("headingsDiff: ");
    Serial.println(headingsDiff);
    
    if (headingsDiff <= targetRangeTight) {  //if your heading is close enough to the target heading, then make something happen

      Serial.println("THAT WAY!");

      hbDelay = 10;

    }
    else  if (headingsDiff <= targetRangeLoose) {
       hbDelay = 1500;
    }
    else {  // you're still off target

      hbDelay = 0;
    }



    HBTime = millis();

    if (HBTime - previousHBTime > hbDelay) { // if we've waited long enough increment the LED brightness
      previousHBTime = HBTime;  // save the time
      if( hbDelay == 10 )
      {
        panicbeat();
      }
      else if( hbDelay == 1500)
      {
        heartbeat();
      }


      // do heartbeat stuff here
      //heartbeat();
      // randNumber = random(200);
      //delay(randNumber);


    }


 // }  // end all the feedback stuff


}





void getHeading () {

  sensors_event_t event;
  mag.getEvent(&event);

  float heading = atan2(event.magnetic.y, event.magnetic.x);
  heading += declinationAngle;



  // Correct for when signs are reversed.
  if (heading < 0)
    heading += 2 * PI;

  // Check for wrap due to addition of declination.
  if (heading > 2 * PI)
    heading -= 2 * PI;

  // Convert radians to degrees for readability.
  headingDegrees = heading * 180 / M_PI;

    Serial.print("Heading (degrees): ");
    Serial.println(headingDegrees);

}






void getGPS() {

  /*====================================================================
    This section of code gets the data from the GPS module - do not edit
    ====================================================================*/
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;
  if (newData == false) {
    // For one second we parse GPS data and report some key values
    for (unsigned long start = millis(); millis() - start < 1000;)
    {
      while (gpsmodule.available())
      {
        char c = gpsmodule.read();
        // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
        if (gps.encode(c)) // Did a new valid sentence come in?
          newData = true;
      }
    }

    if (newData)
    {
      Serial.print("time: ");
      gps.get_datetime(&date, &time, &age); // Use this line of code to get the time in hhmmsscc and date in ddmmyy formats
      Serial.print(time);
      Serial.print(", ");  // You can then work with the "time" and "date" variables in your code

      Serial.print("pos (f): ");
      gps.f_get_position(&flat, &flon, &age);  // Use this line of code to get the latitude and longitude values with a decimal point eg 54.117769
      Serial.print(flat, 6);
      Serial.print(", ");
      Serial.print(flon, 6);
      Serial.print(", "); // You can then work with the "flat" and "flon" variables in your code

      //    Serial.print("pos: ");
      //    gps.get_position(&lat, &lon, &age); // Use this line of code to get the latitude and longitude values without a decimal point eg 5411776. This uses a lot less memory and may be easier for some calculations
      //    Serial.print(lat);
      ///    Serial.print(", ");
      //    Serial.print(lon);
      //    Serial.print(", "); // You can then work with the "lat" and "lon" variables in your code

      //     Serial.print("getGPS dist between: ");
      //     Serial.print(gps.distance_between(flat, flon, fromLat, fromLon)); // Use this code to get the distance between where you are and a given location.


      //  Serial.print("speed: ");
      //  Serial.print(gps.f_speed_mps());
      //   Serial.print(", "); // Use gps.f_speed_mps() to get the speed to 2 decimal places, eg 1.23 metres per second
      // you can also use these for different units:
      //gps.f_speed_mph(); // speed in miles/hr
      //gps.f_speed_kmph(); // speed in km/hr



      //  Serial.print("sats: ");
      //   Serial.print(gps.satellites());
      //   Serial.print(", "); // Use "gps.satellites()" to get the number of satellites in view

      //   Serial.print("hdop: ");
      //   Serial.print(gps.hdop());  // Use "gps.hdop()" to get a measure of how accurate the position is: the smaller this value is the more confidence you have in the accuracy of the location reading
      //    Serial.print(", ");

      //  Serial.print("alt: ");
      //  Serial.print(gps.f_altitude());  // Use "gps.f_altitude()" to get altidude in metres to 2 dp, eg  27.90
      //   Serial.print(", ");

      //   Serial.print("course: ");
      //   Serial.print(gps.f_course());  // Use (gps.f_course() to get your course in degrees to 2 dp, eg 191.76
      //   Serial.print(", ");

      //Serial.print("course to: ");
      //Serial.print(gps.course_to(flat, flon, fromLat, fromLon)); // Use this code to get the distance in metres to the location at fromLat and fromLon
      Serial.println();







      Serial.println();

    } //end newData
  }

} //end getGPS

void heartbeat() {

  //duh-dunk
  Serial.println("duh-dunk");
  digitalWrite(solenoidPin, HIGH);
  delay(10);              //these are the delay values I'd like ideally, but it affects the values output by gpsdump
  digitalWrite(solenoidPin, LOW);
  delay(150);
  randNumber = random(30);
  delay(randNumber);
  digitalWrite(solenoidPin, HIGH);
  delay(10);
  digitalWrite(solenoidPin, LOW);
}

void panicbeat() {
  
    //duh-dunk
  Serial.println("duh-dunk");
  digitalWrite(solenoidPin, HIGH);
  delay(10);              //these are the delay values I'd like ideally, but it affects the values output by gpsdump
  digitalWrite(solenoidPin, LOW);
 
}

