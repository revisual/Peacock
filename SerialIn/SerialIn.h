

#ifndef SerialIn_H
#define SerialIn_H

#include <Arduino.h>


typedef void (*serial_callback)(String);

class SerialIn {

  public:

    // constructor
    SerialIn();

    const static int ACTION_NONE = -1;

    // this function must be called inside loop()
    void run();

    void setCallbacks(char start, serial_callback f1, serial_callback f2);
    void setEndChar(char end);


  private:
    // deferred call constants
    const static byte MAX_ACTIONS = 5;

    // pointers to the callback functions
    int _currentIndex = ACTION_NONE ;
    byte _numbStates = 0 ;
    char _startChar[MAX_ACTIONS];
    char _endChar = '$';
    serial_callback _startCallbacks[MAX_ACTIONS];
    serial_callback _endCallbacks[MAX_ACTIONS];
    String _inputString = "";
    int indexOfInStart( char c);
    void onStartChar();
    void onEndChar();

};

#endif
