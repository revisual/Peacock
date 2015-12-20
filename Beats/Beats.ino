
int _beatPin = 9;

unsigned long _timerStart = 0;
unsigned long _currentDelay = 0;
unsigned int _currentCount = 0;

const int NUMB_CYCLES =  4;

int _currentCycles[NUMB_CYCLES];



 enum BEAT_STATE {
      INACTIVE,
      OFF,
      ON
    };
    BEAT_STATE _beatState = INACTIVE;

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  pinMode(solenoidPin, OUTPUT);
  start(20, 200, 20, 2000);
}

void loop() {
  // put your main code here, to run repeatedly:
  advance();
}

void start(int one, int two, int three, int four)
{
  Serial.println("START");
  _currentCycles[0] =  one;
  _currentCycles[1] =  two;
  _currentCycles[2] =  three;
  _currentCycles[3] =  four;
  
  _beatState = ON;
  setTimer();
  on();
}

void stop()
{
  _beatState = INACTIVE;
  _currentCount = 0;
  off();
}



void advance()
{
    if( _beatState == INACTIVE ) return;

     if( _beatState == ON && (millis() - _timerStart) > _currentDelay) 
     {
      Serial.println("SWITCH OFF");
       _beatState = OFF;
       
        setTimer();
        off();
     }

     else  if( _beatState == OFF && (millis() - _timerStart) > _currentDelay) 
     {
      Serial.println("SWITCH ON");
       _beatState = ON;
        setTimer();
        on();
     }
}

void setTimer()
{
  _timerStart = millis();
  _currentDelay = _currentCycles[_currentCount];

  _currentCount++;
  if( _currentCount >= NUMB_CYCLES)
  {
    _currentCount = 0;
  }
}
void on()
{
  digitalWrite(solenoidPin, HIGH);
}

void off()
{
  digitalWrite(solenoidPin, LOW);
}

