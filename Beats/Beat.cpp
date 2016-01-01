#include "Arduino.h"
#include "Beat.h"

Beat::Beat(byte pin)
{
  _beatPin = pin;
  pinMode(_beatPin, OUTPUT);
}

void Beat::configureState(byte state, int one, int two, int three, int four)
{
  _cyclemap[state][0] = one;
  _cyclemap[state][1] = two;
  _cyclemap[state][2] = three;
  _cyclemap[state][3] = four;
}

void Beat::setState(byte state)
{
  if (_currentBeat == state ) return;

  if ( state == INACTIVE ||  state > MAX_STATES - 1 ||  _cyclemap[state] == NULL)
  {
    stop();
  }

  else
  {
    bool isStart = ( _beatState == INACTIVE || _currentBeat == INACTIVE);

    _currentBeat = state;

    if ( isStart)
    {
      _beatState = ON;
      setTimer();
      on();
    }

  }

}

void Beat::stop()
{
  _beatState = INACTIVE;
  _currentBeat = INACTIVE;
  _currentCount = 0;
  off();
}

void Beat::advance()
{
  if ( _beatState == INACTIVE ) return;

  if ( _beatState == ON && (millis() - _timerStart) > _currentDelay)
  {
    _beatState = OFF;
    setTimer();
    off();
  }

  else if ( _beatState == OFF && (millis() - _timerStart) > _currentDelay)
  {
    _beatState = ON;
    setTimer();
    on();
  }
}

void Beat::on()
{
  digitalWrite(_beatPin, HIGH);
}

void Beat::off()
{
  digitalWrite(_beatPin, LOW);
}

void Beat::setTimer()
{
  _timerStart = millis();
  _currentDelay = _cyclemap[(int)_currentBeat][_currentCount];

  _currentCount++;
  if ( _currentCount >= NUMB_CYCLES)
  {
    _currentCount = 0;
  }
}
