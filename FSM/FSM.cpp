
#include "FSM.h"

FSM::FSM() {

  for (int i = 0; i < MAX_STATES; i++) {

    _enterCallbacks[i] = false;
    _loopCallbacks[i] = false;
  }
}

void FSM::run() {

  if (!_loopCallbacks[_currentState])return;
  (*_loopCallbacks[_currentState])();
}

void FSM::changeState(byte state)
{
  if ( _currentState == state )return;
  _currentState = state;
  (*_enterCallbacks[_currentState])();
}

byte FSM::getCurrentState()
{
  return _currentState;
}

void FSM::setEnterStateCallbacks(byte state, enter_callback f)
{
  _enterCallbacks[state] = f;
}

void FSM::setLoopStateCallbacks(byte state,  loop_callback f)
{
  _loopCallbacks[state] = f;
}


