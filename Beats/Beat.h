/*

*/
#ifndef Beat_h
#define Beat_h

#include "Arduino.h"

class Beat
{
  public:
    Beat(byte pin);

    void configureState( byte state, int one, int two, int three, int four);
    void setState(byte state);
    void stop();
    void advance();
    void on();
    void off();

  private:
    static const byte NUMB_CYCLES =  4;
    static const byte MAX_STATES =  6;
    static const byte INACTIVE =  0;
    static const byte OFF =  1;
    static const byte ON =  2;
    byte _beatPin = 9;
    unsigned long _timerStart = 0;
    unsigned long _currentDelay = 0;
    byte _currentCount = 0;
    int _cyclemap[MAX_STATES][NUMB_CYCLES];
    byte _beatState = INACTIVE;
    byte _currentBeat = 0;
    void setTimer();
};

#endif
