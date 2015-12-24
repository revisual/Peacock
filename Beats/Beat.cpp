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
  Serial.println("setState = " + (String)state);
  if (_currentBeat == state ) return;

  int index = (int)state;

  if ( state == NONE ||  index > MAX_STATES - 1 || index < 0 || _cyclemap[index] == NULL)
  {
    stop();
  }

  else
  {
    bool isStart = ( _beatState == INACTIVE || _currentBeat == NONE);

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
  _currentBeat = NONE;
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
