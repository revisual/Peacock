/*
  
*/

#include "Arduino.h"
#include "Beat.h"

Beat::Beat(int pin)
{
  _beatPin = pin;
  pinMode(_beatPin, OUTPUT);
}

void Beat::start(int one, int two, int three, int four)
{
  _currentCycles[0] =  one;
  _currentCycles[1] =  two;
  _currentCycles[2] =  three;
  _currentCycles[3] =  four;
  _currentCount = 0;

  _beatState = ON;
  setTimer();
  on();
}

void Beat::stop()
{
    _beatState = INACTIVE;
    _currentCount = 0;
    off();
}

void Beat::advance()
{
    if( _beatState == INACTIVE ) return;
        
    if( _beatState == ON && (millis() - _timerStart) > _currentDelay)
    {
        _beatState = OFF;
        setTimer();
        off();
    }
        
    else if( _beatState == OFF && (millis() - _timerStart) > _currentDelay)
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
    if( _currentCount >= NUMB_CYCLES)
    {
        _currentCount = 0;
    }
}
