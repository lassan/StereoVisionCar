int QRE1113_Pin = 2; //connected to digital 2
int _pulseCount = 0;
int _circumference = 0.1; //in meters
int _speed = 0;

int _prevQRE = 3000;

void setup(){
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  
  Serial.begin(57600);
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
}

ISR(TIMER1_COMPA_vect){
    _speed = _pulseCount * _circumference;
    _pulseCount = 0;
}

//answer with speed on an input
void serialEvent() {
  while(Serial.available())
  {
     Serial.read(); 
  }
    Serial.println(_speed);
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
