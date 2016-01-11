
#include <SerialIn.h>

SerialIn _serialIn;

void setup() {
  _serialIn.setCallbacks('~', onSerialLogStart, onSerialLogEnd);
  _serialIn.setCallbacks('@', onSerialPlayStart, onSerialPlayEnd);

}

void onSerialLogStart( String input)
{
  Serial.println("LOG START :: " + input);
}

void onSerialLogEnd( String input)
{
  Serial.println("LOG END :: " + input);
}

void onSerialPlayStart( String input)
{
  Serial.println("PLAY START :: " + input);
}

void onSerialPlayEnd( String input)
{
  Serial.println("PLAY END :: " + input);
}

void serialEvent() {
  _serialIn.run();
}

