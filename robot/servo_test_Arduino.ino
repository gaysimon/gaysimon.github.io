#include <Servo.h>

const int servoPin = 9; // set servo pin number

Servo servo1; // instance of servomotor

void setup(void) {

  // set output pin and attach servo instance to pin
  pinMode(servoPin, OUTPUT);
  servo1.attach(servoPin);


  // move servo from 90 to 0
  for (int i=90;i>=0;i-=5){
    servo1.write(i);
    delay(20);
  }

  // move servo from 0 to 180
  for (int i=0;i<=180;i++){
    servo1.write(i);
    delay(20);
  }

  // move servo from 180 to 90
  for (int i=180;i>=90;i--){
    servo1.write(i);
    delay(20);
  }

  servo1.write(90);
}


void loop(void) {
  
}
