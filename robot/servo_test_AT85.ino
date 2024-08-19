
const int servoPin = 0; // set servo pin number (on ATTiny 85 : 0 to 4)

int angle=1500;


void setup(void) {

  // set input and output pins
  pinMode(servoPin, OUTPUT);
  pinMode(5, INPUT);  // reset pin

  digitalWrite(servoPin, LOW);

  // move servo from 90 to 0
  for (int i=0;i<=100;i++){
    angle=1500-5*i;           // servomotor pulse signal
    digitalWrite(servoPin, HIGH);
    delayMicroseconds(angle);
    digitalWrite(servoPin, LOW);
    delay(19);
  }

  // move servo from 0 to 180
  for (int i=0;i<=200;i++){
    angle=1000+5*i;
    digitalWrite(servoPin, HIGH);
    delayMicroseconds(angle);
    digitalWrite(servoPin, LOW);
    delay(19);
  }

  // move servo from 180 to 90
  for (int i=0;i<=100;i++){
    angle=2000-5*i;
    digitalWrite(servoPin, HIGH);
    delayMicroseconds(angle);
    digitalWrite(servoPin, LOW);
    delay(19);
  }
}


void loop(void) {
  
  // maintain position 90
  digitalWrite(servoPin, HIGH);
  delayMicroseconds(1500);
  digitalWrite(servoPin, LOW);
  delay(19);
}
