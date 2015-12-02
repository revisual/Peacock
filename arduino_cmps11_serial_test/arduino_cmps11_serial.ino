/*****************************************
*   CMPS11 Serial example for Arduino    *
*        By James Henderson, 2014        * 
*****************************************/

#include <SoftwareSerial.h>

#define CMPS_GET_ANGLE8 0x12
#define CMPS_GET_ANGLE16 0x13
#define CMPS_GET_PITCH 0x14
#define CMPS_GET_ROLL 0x15

SoftwareSerial cmps11 = SoftwareSerial(5,6);

unsigned char high_byte, low_byte, angle8;
char pitch, roll;
unsigned int angle16;

void setup()
{
  Serial.begin(9600);              // Start serial ports
  cmps11.begin(9600);
}

void loop()
{
  cmps11.write(CMPS_GET_ANGLE16);  // Request and read 16 bit angle
  while(cmps11.available() < 2);
  high_byte = cmps11.read();
  low_byte = cmps11.read();
  angle16 = high_byte;                // Calculate 16 bit angle
  angle16 <<= 8;
  angle16 += low_byte;
  
  cmps11.write(CMPS_GET_ANGLE8);  // Request and read 8 bit angle
  while(cmps11.available() < 1);
  angle8 = cmps11.read();
  
  cmps11.write(CMPS_GET_PITCH);   // Request and read pitch value
  while(cmps11.available() < 1);
  pitch = cmps11.read();
  
  cmps11.write(CMPS_GET_ROLL);    // Request and read roll value
  while(cmps11.available() < 1);
  roll = cmps11.read();
  
  Serial.print("roll: ");            // Display roll data
  Serial.print(roll, DEC);
  
  Serial.print("    pitch: ");          // Display pitch data
  Serial.print(pitch, DEC);
  
  Serial.print("    angle full: ");       // Display 16 bit angle with decimal place
  Serial.print(angle16 / 10, DEC);
  Serial.print(".");
  Serial.print(angle16 % 10, DEC);
  
  Serial.print("    angle 8: ");        // Display 8bit angle
  Serial.println(angle8, DEC);
  
  delay(100);                           // Short delay before next loop
}




