const int QRE1113_Pin = 2;
const int brakeLEDPin = 13;
const int leftIndicatorPin = 3;
const int rightIndicatorPin = 4;

boolean leftIndicatorOn = false;
boolean rightIndicatorOn = false;

int leftIndicatorStatus = LOW;
int rightIndicatorStatus = LOW;
unsigned long indicatorTimer;
long previousMillis = 0;
long indicatorInterval = 250UL;

int _pulseCount = 0;
double _circumference = 0.1; //in meters
double _speed = 0;

int _prevQRE = 3000;

void setup(){
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 2hz increments
  OCR1A = 7812;// = (16*10^6) / (2*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  
  pinMode(brakeLEDPin, OUTPUT);
  
  Serial.begin(56700);
}


void loop(){

  int QRE_Value = readQD();
  if(QRE_Value < 500){
     if(_prevQRE > 500) 
     {
        _pulseCount++; 
     }
  } 
  _prevQRE = QRE_Value;
  
  leftIndicator();
  rightIndicator();
}

ISR(TIMER1_COMPA_vect){
    _speed = _pulseCount * _circumference * 2;   
    _pulseCount = 0;
}

//answer with speed on an input
void serialEvent() {
  byte inByte;
  
  while(Serial.available())
  {
    byte inByte = Serial.read(); 
     
    switch(inByte) 
    {
      case 0x00: //send speed
         Serial.println(_speed);
         break;
      case 0x01: //turn brake light on
         digitalWrite(brakeLEDPin, HIGH);
         break;
     case 0x02:  //turn brake light off
         digitalWrite(brakeLEDPin, LOW);
         break; 
     case 0x03:  //turn left inidicator on
         leftIndicatorOn = true;
         break;
     case 0x04:  //turn left indicator off
         leftIndicatorOn = false;
         break;
     case 0x05:  //turn right indicator on
         rightIndicatorOn = true;
         break;
     case 0x06:  //turn right indicator off
         rightIndicatorOn = false;
         break;
     
     default:
         break;
    }
  }
}

void leftIndicator()
{
  
  if(leftIndicatorOn)
  {
      indicatorTimer = millis();
      if(indicatorTimer - previousMillis > indicatorInterval)
      {
        if(leftIndicatorStatus == LOW)
          leftIndicatorStatus = HIGH;
        else 
          leftIndicatorStatus = LOW;
      }
  } else leftIndicatorStatus = LOW;
  digitalWrite(leftIndicatorPin, leftIndicatorStatus);
}


void rightIndicator()
{
  if(rightIndicatorOn)
  {
      indicatorTimer = millis();
      if(indicatorTimer - previousMillis > indicatorInterval)
      {
        if(rightIndicatorStatus == LOW)
          rightIndicatorStatus = HIGH;
        else 
          rightIndicatorStatus = LOW;
      }
  } else rightIndicatorStatus = LOW;
  digitalWrite(rightIndicatorPin, rightIndicatorStatus);
}

int readQD(){
  //Returns value from the QRE1113 
  //Lower numbers mean more refleacive
  //More than 3000 means nothing was reflected.
  pinMode( QRE1113_Pin, OUTPUT );
  digitalWrite( QRE1113_Pin, HIGH );  
  delayMicroseconds(10);
  pinMode( QRE1113_Pin, INPUT );

  long time = micros();

  //time how long the input is HIGH, but quit after 3ms as nothing happens after that
  while (digitalRead(QRE1113_Pin) == HIGH && micros() - time < 3000); 
  int diff = micros() - time;

  return diff;
}
