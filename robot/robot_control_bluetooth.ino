
#define speedPinRF 9            //RIGHT WHEEL PWM pin D45 connect front MODEL-X ENA 
#define RightMotorDirPin1F  22  //Front Right Motor direction pin 1 to Front MODEL-X IN1  (K1)
#define RightMotorDirPin2F  24  //Front Right Motor direction pin 2 to Front MODEL-X IN2   (K1)    
#define speedPinLF 10           //Left WHEEL PWM pin D7 connect front MODEL-X ENB                             
#define LeftMotorDirPin1F  26   //Left front Motor direction pin 1 to Front MODEL-X IN3 (  K3)
#define LeftMotorDirPin2F  28   //Left front Motor direction pin 2 to Front MODEL-X IN4 (  K3)


#define speedPinRB 11           //RIGHT WHEEL PWM pin connect Back MODEL-X ENA 
#define RightMotorDirPin1B  5   //Rear Right Motor direction pin 1 to Back MODEL-X IN1 (  K1)
#define RightMotorDirPin2B 6    //Rear Right Motor direction pin 2 to Back MODEL-X IN2 (  K1) 
#define speedPinLB 12           //LEFT WHEEL  PWM pin D8 connect Rear MODEL-X ENB
#define LeftMotorDirPin1B 7     //Rear left Motor direction pin 1 to Back MODEL-X IN3  K3
#define LeftMotorDirPin2B 8     //Rear left Motor direction pin 2 to Back MODEL-X IN4  k3

int code=0;

byte character=0;
int donneesALire = 0;

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
  Serial1.begin(9600);  // bluetooth serial
  Serial.begin(9600);   // USB serial
}

void loop() {
   Serial1.println(" test ");

  char character='a';
  int donneesALire = Serial1.available(); // detect characters in buffer
  
  int vx=0;
  int vy=0;
  int rz=0;

  int complete=0;

  if(donneesALire > 0){

    while (donneesALire>0){
      character = Serial1.read();
      if (character<0) character+=256;
      donneesALire = Serial1.available();

      // if character is 255, start a new code
      if (((int)character+256)%256==255){
        code=1;
      }
      else{
        if (code==0){
          // just read the character
        }
        else if (code==1){
          vx=((int)character+256)%256-100;  // read x-speed
          code=2;
        }
        else if (code==2){
          vy=((int)character+256)%256-100;  // read y-speed
          code=3;
        }
        else if (code==3){
          rz=((int)character+256)%256-100;  // read z-rotation speed 
          code=0;
          complete=1;
        }
      }
    }
  }


  if (complete==1){
    complete=0;
    Serial.print(vx);
    Serial.print(", ");
    Serial.print(vy);
    Serial.print(", ");
    Serial.println(rz);

    // compute motor speed
    int speedR1=(-vx-vy+rz)*2;
    int speedL1=(vx-vy-rz)*2;
    int speedR2=(vx-vy+rz)*2;
    int speedL2=(-vx-vy-rz)*2;

    // bound values
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

    // set motor values
    frontRight(speedR1);
    frontLeft(speedL1);
    backRight(speedR2);
    backLeft(speedL2);
  }
}
