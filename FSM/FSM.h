

#ifndef FSM_H
#define FSM_H

typedef void (*enter_callback)(void);
typedef void (*loop_callback)(void);

class FSM {

  public:

    // constructor
    FSM();

    // this function must be called inside loop()
    void run();

    byte getCurrentState();

    void setEnterStateCallbacks(byte state, enter_callback f);
    void setLoopStateCallbacks(byte state, loop_callback f);

    void changeState(byte state);

  private:

    const static byte MAX_STATES = 10;

    enter_callback _enterCallbacks[MAX_STATES];
    loop_callback _loopCallbacks[MAX_STATES];
    byte _currentState = 0;

};

#endif
