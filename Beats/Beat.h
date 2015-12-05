/*
 
*/
#ifndef Beat_h
#define Beat_h

#include "Arduino.h"

class Beat
{
  public:
    Beat(int pin);
    void start(int one, int two, int three, int four);
    void stop();
    void advance();
    void on();
    void off();

  private:
    static const int NUMB_CYCLES =  4;
    int _beatPin = 9;
    unsigned long _timerStart = 0;
    unsigned long _currentDelay = 0;
    unsigned int _currentCount = 0;
    int _currentCycles[4];
    enum state {
                         INACTIVE,
                         OFF,
                         ON
                        };
        state _beatState = INACTIVE;
    void setTimer();
};

#endif