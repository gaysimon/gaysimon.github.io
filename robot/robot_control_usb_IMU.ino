
#include <Wire.h>//https://www.arduino.cc/en/reference/wire
#include <Adafruit_MPU6050.h>//https://github.com/adafruit/Adafruit_MPU6050
#include <Adafruit_Sensor.h>//https://github.com/adafruit/Adafruit_Sensor

#define speedPinRF 9   //  RIGHT WHEEL PWM pin D45 connect front MODEL-X ENA 
#define RightMotorDirPin1F  22    //Front Right Motor direction pin 1 to Front MODEL-X IN1  (K1)
#define RightMotorDirPin2F  24   //Front Right Motor direction pin 2 to Front MODEL-X IN2   (K1)    
#define speedPinLF 10   // Left WHEEL PWM pin D7 connect front MODEL-X ENB                             
#define LeftMotorDirPin1F  26    //Left front Motor direction pin 1 to Front MODEL-X IN3 (  K3)
#define LeftMotorDirPin2F  28   //Left front Motor direction pin 2 to Front MODEL-X IN4 (  K3)


#define speedPinRB 11   //  RIGHT WHEEL PWM pin connect Back MODEL-X ENA 
#define RightMotorDirPin1B  5    //Rear Right Motor direction pin 1 to Back MODEL-X IN1 (  K1)
#define RightMotorDirPin2B 6    //Rear Right Motor direction pin 2 to Back MODEL-X IN2 (  K1) 
#define speedPinLB 12    //   LEFT WHEEL  PWM pin D8 connect Rear MODEL-X ENB
#define LeftMotorDirPin1B 7    //Rear left Motor direction pin 1 to Back MODEL-X IN3  K3
#define LeftMotorDirPin2B 8  //Rear left Motor direction pin 2 to Back MODEL-X IN4  k3

Adafruit_MPU6050 mpu;

long time1;
long time2;

int code=0;

byte character=0;
int donneesALire = 0;

int vx=0;
int vy=0;
int rz=0;

void frontRight(int speed){
  if (speed==0){
    analogWrite(speedPinRF,0);
    digitalWrite(RightMotorDirPin1F,LOW);
    digitalWrite(RightMotorDirPin2F,LOW); 
  }
  else if (speed>0){
    digitalWrite(RightMotorDirPin1F,LOW);
    digitalWrite(RightMotorDirPin2F,HIGH); 
    analogWrite(speedPinRF,speed);
  }
  else{
    digitalWrite(RightMotorDirPin1F,HIGH);
    digitalWrite(RightMotorDirPin2F,LOW); 
    analogWrite(speedPinRF,-speed);
  }
}

void frontLeft(int speed){
  if (speed==0){
    analogWrite(speedPinLF,0);
    digitalWrite(LeftMotorDirPin1F,LOW);
    digitalWrite(LeftMotorDirPin2F,LOW); 
  }
  else if (speed>0){
    digitalWrite(LeftMotorDirPin1F,LOW);
    digitalWrite(LeftMotorDirPin2F,HIGH); 
    analogWrite(speedPinLF,speed);
  }
  else{
    digitalWrite(LeftMotorDirPin1F,HIGH);
    digitalWrite(LeftMotorDirPin2F,LOW); 
    analogWrite(speedPinLF,-speed);
  }
}


void backRight(int speed){
  if (speed==0){
    analogWrite(speedPinRB,0);
    digitalWrite(RightMotorDirPin1B,LOW);
    digitalWrite(RightMotorDirPin2B,LOW); 
  }
  else if (speed>0){
    digitalWrite(RightMotorDirPin1B,LOW);
    digitalWrite(RightMotorDirPin2B,HIGH); 
    analogWrite(speedPinRB,speed);
  }
  else{
    digitalWrite(RightMotorDirPin1B,HIGH);
    digitalWrite(RightMotorDirPin2B,LOW); 
    analogWrite(speedPinRB,-speed);
  }
}

void backLeft(int speed){
  if (speed==0){
    analogWrite(speedPinLB,0);
    digitalWrite(LeftMotorDirPin1B,LOW);
    digitalWrite(LeftMotorDirPin2B,LOW); 
  }
  else if (speed>0){
    digitalWrite(LeftMotorDirPin1B,LOW);
    digitalWrite(LeftMotorDirPin2B,HIGH); 
    analogWrite(speedPinLB,speed);
  }
  else{
    digitalWrite(LeftMotorDirPin1B,HIGH);
    digitalWrite(LeftMotorDirPin2B,LOW); 
    analogWrite(speedPinLB,-speed);
  }
}



void setup() {

  Serial.begin(9600);   // USB serial

  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  
  time1=micros();
  time2=micros();
}

void loop() {
  readMPU();
  controlMotors();
}


void controlMotors(){

  char character='a';
  int donneesALire = Serial.available(); // detect characters in buffer
  
  int complete=0;

  if(donneesALire > 0){

    while (donneesALire>0){
      character = Serial.read();
      if (character<0) character+=256;
      donneesALire = Serial.available(); 
   
      if (((int)character+256)%256==255){
        code=1;
      }
      else{
        if (code==0){
          // just read the character
        }
        else if (code==1){
          vx=((int)character+256)%256-100;
          code=2;
        }
        else if (code==2){
          vy=((int)character+256)%256-100;
          code=3;
        }
        else if (code==3){
          rz=((int)character+256)%256-100;
          code=0;
          complete=1;
        }
      }
    }
  }
  
  if (complete==1){ // once a command is complete, set motor speed
    complete=0;

    vx=vx/4;
    vy=vy/4;
    rz=rz/10;

    int speedR1=(-vx-vy+rz)*4;
    int speedL1=(vx-vy-rz)*4;
    int speedR2=(vx-vy+rz)*4;
    int speedL2=(-vx-vy-rz)*4;

    if (speedR1>0) speedR1+=50;
    if (speedR1<0) speedR1-=50;
    if (speedR1> 250) speedR1=250;
    if (speedR1<-250) speedR1=-250;

    if (speedL1>0) speedL1+=50;
    if (speedL1<0) speedL1-=50;
    if (speedL1> 250) speedL1=250;
    if (speedL1<-250) speedL1=-250;

    if (speedR2>0) speedR2+=50;
    if (speedR2<0) speedR2-=50;
    if (speedR2> 250) speedR2=250;
    if (speedR2<-250) speedR2=-250;
 
    if (speedL2>0) speedL2+=50;
    if (speedL2<0) speedL2-=50;
    if (speedL2> 250) speedL2=250;
    if (speedL2<-250) speedL2=-250;

    frontRight(speedR1);
    frontLeft(speedL1);
    backRight(speedR2);
    backLeft(speedL2);
  }  
}

void readMPU( ) { /* function readMPU */
  ////Read acceleromter data
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  time1=micros();

  Serial.print((char)255);

  // time
  int val=time1-time2;
  int val1=val/254;
  int val2=val%254;
  Serial.print(char(val1));
  Serial.print(char(val2));

  // accel X
  val=(int)( (a.acceleration.x)*100)+32258;
  val1=val/254;
  val2=val%254;
  Serial.print(char(val1));
  Serial.print(char(val2));
  
  // accel Y
  val=(int)( (a.acceleration.y)*100)+32258;
  val1=val/254;
  val2=val%254;
  Serial.print(char(val1));
  Serial.print(char(val2));

  // accel Z
  val=(int)( (a.acceleration.z)*100)+32258;
  val1=val/254;
  val2=val%254;
  Serial.print(char(val1));
  Serial.print(char(val2));

  // rot x
  val=(int)(g.gyro.x*100)+32258;
  val1=val/254;
  val2=val%254;
  Serial.print(char(val1));
  Serial.print(char(val2));

  // rot y
  val=(int)(g.gyro.y*100)+32258;
  val1=val/254;
  val2=val%254;
  Serial.print(char(val1));
  Serial.print(char(val2));

  // rot Z
  val=(int)(g.gyro.z*100)+32258;
  val1=val/254;
  val2=val%254;
  Serial.print(char(val1));
  Serial.print(char(val2));

  time2=time1;
}
