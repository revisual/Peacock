
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
  if (!_enterCallbacks[_currentState])return;
  (*_enterCallbacks[_currentState])();
}

byte FSM::getCurrentState()
{
  return _currentState;
}

void FSM::setEnterStateCallbacks(byte state, fsm_callback f)
{
  _enterCallbacks[state] = f;
}

void FSM::setLoopStateCallbacks(byte state,  fsm_callback f)
{
  _loopCallbacks[state] = f;
}

