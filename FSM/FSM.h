

#ifndef FSM_H
#define FSM_H

#include <Arduino.h>

typedef void (*fsm_callback)(void);

class FSM {

  public:

    // constructor
    FSM();

    // this function must be called inside loop()
    void run();

    byte getCurrentState();

    void setEnterCallback(byte state, fsm_callback f);
    void setRunCallback(byte state, fsm_callback f);

    void changeState(byte state);

  private:

    const static byte MAX_STATES = 10;

    fsm_callback _enterCallbacks[MAX_STATES];
    fsm_callback _runCallbacks[MAX_STATES];
    byte _currentState = 0;

};

#endif
