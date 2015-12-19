#include "Arduino.h"
#include "Beat.h"

Beat::Beat(int pin)
{
  _beatPin = pin;
  pinMode(_beatPin, OUTPUT);
}

void Beat::configureState(enum BeatType state, int one, int two, int three, int four)
{
  int index = (int)state;
  _cyclemap[index][0] = one;
  _cyclemap[index][1] = two;
  _cyclemap[index][2] = three;
  _cyclemap[index][3] = four;
}

void Beat::setState(enum BeatType state)
{
  int index = (int)state;

  if (_currentBeat == state ) return;

  if ( state == NONE || index > MAX_STATES - 1 || index < 0 || _cyclemap[index] == NULL)
  {
    stop();
  }
  
  else
  {
    _currentBeat = state;

    _currentCycles[0] = _cyclemap[index][0];
    _currentCycles[1] = _cyclemap[index][1];
    _currentCycles[2] = _cyclemap[index][2];
    _currentCycles[3] = _cyclemap[index][3];

    _currentCount = 0;
    _beatState = ON;
    setTimer();
    on();
  }

}

void Beat::stop()
{
  _beatState = INACTIVE;
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
  _currentDelay = _currentCycles[_currentCount];

  _currentCount++;
  if ( _currentCount >= NUMB_CYCLES)
  {
    _currentCount = 0;
  }
}
