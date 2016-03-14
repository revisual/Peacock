
/***************************************************
  This is an example for the Adafruit VS1053 Codec Breakout

  Designed specifically to work with the Adafruit VS1053 Codec Breakout
  ----> https://www.adafruit.com/products/1381

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <SerialIn.h>

// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

// These are the pins used for the music maker shield
#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)
#define CARDCS        4     // Card chip select pin
#define DREQ          3       // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer =
  Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

static const char LOG_CHAR = '~';
static const char PLAY_CHAR = '@';
static const char PIPE_CHAR = '|';
static const char END_CHAR = '$';

SerialIn serialIn;


void setup() {
  // set up serial port
  Serial.begin(9600);

  if (! musicPlayer.begin()) { // initialise the music player
    Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
    while (1);
  }

  SD.begin(CARDCS);    // initialise the SD card

  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(1, 1);
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int

  serialIn.setCallbacks('@', onSerialPlayStart, onSerialPlayEnd);
  serialIn.setCallbacks('#', onSerialCommandStart, onSerialCommandEnd);
  serialIn.setCallbacks('~', onSerialLogStart, onSerialLogEnd);

  log(F("S:start"));
}

void loop()
{

}

void serialEvent()
{
  serialIn.run();
}

void onSerialLogStart( String input)
{

}

void onSerialLogEnd( String input)
{
  log(input);
}

void onSerialCommandStart( String input)
{

}

void onSerialCommandEnd( String input)
{
  File file = SD.open(input);
  if (file) {
    while (file.available()) {
      Serial.write(file.read());
    }
    file.close();
  }
  log(F("------------------"));
  log(F("S:SENT"));
  log(input);
}

void onSerialPlayStart( String input)
{

}

void onSerialPlayEnd( String input)
{
  //Serial.println("PLAY END :: " + input);
  char buff[input.length() + 1] ;
  input.toCharArray(buff, input.length() + 1);
  playFile(buff);
}

void playFile(char *name) {
  if (!SD.exists(name))
  {
    log(F("!!!!!!!!!!!!!!!!!!!"));
    log(F("S:NOTPLAYING"));
    log(name);
  }
  else
  {
    musicPlayer.stopPlaying();
    log(F("------------------"));
    log(F("S:PLAYING"));
    log(name);
    musicPlayer.startPlayingFile(name);
  }

}

void log(String dataString)
{
  if (!musicPlayer.stopped())return;
  File file = SD.open(F("log.txt"), FILE_WRITE);
  if ( file )
  {
    file.println(dataString);
    file.close();
    Serial.println(dataString);
  }

}

