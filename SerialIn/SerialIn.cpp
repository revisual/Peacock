
#include "SerialIn.h"

SerialIn::SerialIn() {

  for (int i = 0; i < MAX_ACTIONS; i++) {

    _startCallbacks[i] = false;
    _endCallbacks[i] = false;
  }
}

void SerialIn::setCallbacks(char start, serial_callback f1, serial_callback f2)
{
  _startChar[_numbStates] = start;
  _startCallbacks[_numbStates] = f1;
  _endCallbacks[_numbStates] = f2;

  _numbStates++;
}

void SerialIn::setEndChar(char end)
{
  _endChar = end;
}

void SerialIn::run()
{
  while (Serial.available()) {

    char inChar = (char)Serial.read();

    if (_currentIndex == ACTION_NONE)
    {
      _currentIndex  = indexOfInStart(inChar);
    }

    else
    {
      if ( inChar == _startChar[_currentIndex])
      {
        onStartChar();
      }

      else  if ( inChar == _endChar)
      {
        onEndChar();
      }

      else
      {
        _inputString += inChar;
      }
    }
  }
}

void SerialIn::onStartChar()
{
  if (!_startCallbacks[_currentIndex])return;
  (*_startCallbacks[_currentIndex])(_inputString);
  _inputString = "";
}

void SerialIn::onEndChar()
{
  if (!_endCallbacks[_currentIndex])return;
  (*_endCallbacks[_currentIndex])(_inputString);
  _currentIndex = ACTION_NONE;
  _inputString = "";
}

int SerialIn::indexOfInStart( char c)
{
  for (int i = 0; i < MAX_ACTIONS; i++)
  {
    if (_startChar[i] == c) return i;
  }
  return ACTION_NONE;
}

