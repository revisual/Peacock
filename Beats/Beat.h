/*

*/
#ifndef Beat_h
#define Beat_h

#include "Arduino.h"

class Beat
{
  public:
    Beat(int pin);

    enum BeatType {
      NONE,
      SLOW_HEART,
      STEADY_HEART,
      RAPID_HEART
    };
    
    void configureState( enum BeatType state, int one, int two, int three, int four);
    void setState(enum BeatType state);
    void stop();
    void advance();
    void on();
    void off();
    
    



  private:
    static const int NUMB_CYCLES =  6;
    static const int MAX_STATES =  4;
    int _beatPin = 9;
    unsigned long _timerStart = 0;
    unsigned long _currentDelay = 0;
    unsigned int _currentCount = 0;
    int _currentCycles[NUMB_CYCLES];
    int _cyclemap[MAX_STATES][NUMB_CYCLES];
    enum state {
      INACTIVE,
      OFF,
      ON
    };
    state _beatState = INACTIVE;
    BeatType _currentBeat = NONE;
    void setTimer();
};

#endif
