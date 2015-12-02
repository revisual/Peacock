#include <SoftwareSerial.h> //includes the software serial library
#include <TinyGPS.h>  // includes the TinyGPS library

/* 
Based on TinyGPS by Mikal Hart
Uses a 4800-baud serial GPS device hooked up on pins 3(rx) and 4(tx).
*/

TinyGPS gps;
SoftwareSerial gpsmodule(3, 4);  // The pins you're communicating with the GPS module on: 3(rx) and 4(tx)

//some variables...

float flat, flon; // +/- latitude/longitude in degrees
unsigned long date, time, age; // what they say on the tin
long lat, lon; // +/- lat/long in 100000ths of a degree
float fromLat = 54.117769; // latitude of a place you want to compare against
float fromLon =  -3.220657; // longitude of a place you want to compare against


void setup()  //this section of code runs once when you start the microcontroller
{
  Serial.begin(115200);  // serial communication between the microcontroller and the computer
  gpsmodule.begin(4800); // serial communication between the gps module and the microcontroller
  
} //end setup


void loop() //this section of code runs ad infinitum, looping until you make it stop
{
 
 
 /*====================================================================
 This section of code gets the data from the GPS module - do not edit
 ====================================================================*/
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (gpsmodule.available())
    {
      char c = gpsmodule.read();
       Serial.print(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }

 /*====================================================================================
 Edit the contents of if(newData) { } with what to do when you get new GPS information
 ====================================================================================*/

  if (newData)
  {
    Serial.print("time: ");
    gps.get_datetime(&date, &time, &age); // Use this line of code to get the time in hhmmsscc and date in ddmmyy formats
    Serial.print(time); Serial.print(", ");  // You can then work with the "time" and "date" variables in your code

    Serial.print("pos (f): ");
    gps.f_get_position(&flat, &flon, &age);  // Use this line of code to get the latitude and longitude values with a decimal point eg 54.117769
    Serial.print(flat, 6);  Serial.print(", ");  Serial.print(flon, 6); Serial.print(", "); // You can then work with the "flat" and "flon" variables in your code
    
    Serial.print("pos: ");
    gps.get_position(&lat, &lon, &age); // Use this line of code to get the latitude and longitude values without a decimal point eg 5411776. This uses a lot less memory and may be easier for some calculations 
    Serial.print(lat);  Serial.print(", ");  Serial.print(lon); Serial.print(", "); // You can then work with the "lat" and "lon" variables in your code
    
    Serial.print("speed: ");
    Serial.print(gps.f_speed_mps()); Serial.print(", "); // Use gps.f_speed_mps() to get the speed to 2 decimal places, eg 1.23 metres per second
    
    Serial.print("dist between: ");
    Serial.print(gps.distance_between(flat, flon, fromLat, fromLon)); // Use this code to get the distance between where you are and a given location. You can either use variables as shown here, or place the values for lat and lon directly in here.
    Serial.print(", ");
    
    Serial.print("sats: ");
    Serial.print(gps.satellites()); Serial.print(", "); // Use "gps.satellites()" to get the number of satellites in view
    
    Serial.print("hdop: ");
    Serial.print(gps.hdop());  // Use "gps.hdop()" to get a measure of how accurate the position is: the smaller this value is the more confidence you have in the accuracy of the location reading
    Serial.print(", ");
    
    Serial.print("alt: ");
    Serial.print(gps.f_altitude());  // Use "gps.f_altitude()" to get altidude in metres to 2 dp, eg  27.90
    Serial.print(", ");
    
    Serial.print("course: ");
    Serial.print(gps.f_course());  // Use (gps.f_course() to get your course in degrees to 2 dp, eg 191.76
    Serial.print(", ");
    
    Serial.print("course to: ");
    Serial.print(gps.course_to(flat, flon, fromLat, fromLon)); // Use this code to get the distance in metres to the location at fromLat and fromLon
    Serial.println(); 

  } //end newData


// you might sometimes also need to add code into here too

  

} //end loop
