#include <FatReader.h>
#include <SdReader.h>
#include <SerialIn.h>
#include <avr/pgmspace.h>
#include "WaveUtil.h"
#include "WaveHC.h"

static const char LOG_CHAR = '~';
static const char PLAY_CHAR = '@';
static const char PIPE_CHAR = '|';
static const char END_CHAR = '$';

static const byte ACTION_NONE = 0;
static const byte ACTION_LOG = 1;
static const byte ACTION_PLAY = 2;
static const byte ACTION_DESERIALISE = 3;

SdReader card;    // This object holds the information for the card
FatVolume vol;    // This holds the information for the partition on the card
FatReader root;   // This holds the information for the filesystem on the card
FatReader f;      // This holds the information for the file we're play
SerialIn serialIn;

byte readingState = ACTION_NONE;
String inputString = "";

WaveHC wave;      // This is the only wave (audio) object, since we will only play one at a time

// this handy function will return the number of bytes currently free in RAM, great for debugging!
int freeRam(void)
{
  extern int  __bss_end;
  extern int  *__brkval;
  int free_memory;
  if ((int)__brkval == 0) {
    free_memory = ((int)&free_memory) - ((int)&__bss_end);
  }
  else {
    free_memory = ((int)&free_memory) - ((int)__brkval);
  }
  return free_memory;
}

void sdErrorCheck(void)
{
  if (!card.errorCode()) return;
  putstring("\n\rSD I/O error: ");
  Serial.print(card.errorCode(), HEX);
  putstring(", ");
  Serial.println(card.errorData(), HEX);
  while (1);
}

void setup() {
  // set up serial port
  Serial.begin(9600);
  //putstring_nl("WaveHC with 6 buttons");

  //putstring("Free RAM: ");       // This can help with debugging, running out of RAM is bad
  //Serial.println(freeRam());      // if this is under 150 bytes it may spell trouble!

  // Set the output pins for the DAC control. This pins are defined in the library
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

  // pin13 LED
  pinMode(13, OUTPUT);

  // enable pull-up resistors on switch pins (analog inputs)
  digitalWrite(14, HIGH);
  digitalWrite(15, HIGH);
  digitalWrite(16, HIGH);
  digitalWrite(17, HIGH);
  digitalWrite(18, HIGH);
  digitalWrite(19, HIGH);

  //  if (!card.init(true)) { //play with 4 MHz spi if 8MHz isn't working for you
  if (!card.init()) {         //play with 8 MHz spi (default faster!)
    putstring_nl("Card init. failed!");  // Something went wrong, lets print out why
    sdErrorCheck();
    while (1);                           // then 'halt' - do nothing!
  }

  // enable optimize read - some cards may timeout. Disable if you're having problems
  card.partialBlockRead(true);

  // Now we will look for a FAT partition!
  uint8_t part;
  for (part = 0; part < 5; part++) {     // we have up to 5 slots to look in
    if (vol.init(card, part))
      break;                             // we found one, lets bail
  }
  if (part == 5) {                       // if we ended up not finding one  :(
    putstring_nl("No valid FAT partition!");
    sdErrorCheck();      // Something went wrong, lets print out why
    while (1);                           // then 'halt' - do nothing!
  }

  // Lets tell the user about what we found
  // putstring("Using partition ");
  //Serial.print(part, DEC);
  //putstring(", type is FAT");
  //Serial.println(vol.fatType(), DEC);    // FAT16 or FAT32?

  // Try to open the root directory
  if (!root.openRoot(vol)) {
    putstring_nl("Can't open root dir!"); // Something went wrong,
    while (1);                            // then 'halt' - do nothing!
  }

  // Whew! We got past the tough parts.
  putstring_nl("Ready!");

  serialIn.setCallbacks('@', onSerialPlayStart, onSerialPlayEnd);
  serialIn.setCallbacks('#', onSerialCommandStart, onSerialCommandEnd);
  serialIn.setCallbacks('~', onSerialLogStart, onSerialLogEnd);
}

void loop() {

}

void serialEvent() {

  serialIn.run();

}



void onSerialLogStart( String input)
{
  //Serial.println("LOG START :: " + input);
}

void onSerialLogEnd( String input)
{
  //Serial.println("LOG END :: " + input);
  log(input);
}

void onSerialCommandStart( String input)
{
  //Serial.println("LOG START :: " + input);
}

void onSerialCommandEnd( String input)
{
  if (input == F("data"))
  {
    Serial.println("COMMAND END :: " + input);
    // send back data here
  }
}

void onSerialPlayStart( String input)
{
  // Serial.println("PLAY START :: " + input);
}

void onSerialPlayEnd( String input)
{
  Serial.println("PLAY END :: " + input);
  char buff[input.length() + 1] ;
  input.toCharArray(buff, input.length() + 1);
  playFile(buff);
}





void playFile(char *name) {

  // see if the wave object is currently doing something
  if (wave.isplaying)
  { // already playing something, so stop it!
    wave.stop(); // stop it
  }
  // look in the root directory and open the file
  if (!f.open(root, name))
  {
    putstring("Couldn't open file "); Serial.print(name); return;
  }
  // OK read the file and turn it into a wave object
  if (!wave.create(f))
  {
    putstring_nl("Not a valid WAV"); return;
  }
  putstring("PLAYING "); Serial.print(name);

  // ok time to play! start playback
  wave.play();
}

void log(String dataString)
{
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  /* File dataFile = SD.open("log.txt", FILE_WRITE);

    // if the file is available, write to it:
    if (dataFile) {
     dataFile.println(dataString);
     dataFile.close();
     // print to the serial port too:
     Serial.println(dataString);
    }
    // if the file isn't open, pop up an error:
    else {
     Serial.println("error opening datalog.txt");
    }
  */
  Serial.println(dataString);
}

