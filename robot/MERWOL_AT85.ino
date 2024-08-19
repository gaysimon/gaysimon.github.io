
// intended and enacted interactions
byte intended=0;
byte enacted=0;
byte timeline=0;

// sensor value and variation
int val1=0;
int val2=0;
int delta=0;

// generic iterator
byte i=0; 



void setup(void) {

  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  
  pinMode(2, INPUT);
  pinMode(5, INPUT);

  digitalWrite(0, LOW);
  digitalWrite(1, LOW);

  delay(200);

  // get first sensor value
  val1=analogRead(1);
  val1+=analogRead(1);
  val1+=analogRead(1);
  val1+=analogRead(1);
  val1=val1>>2;    // divide by 4

  // first intention
  intended=0b00;
}



void loop(void) {

  // enact intended interaction (send servomotor pulses ten times)
  for (i=0;i<10;i++){
    if ( (intended & 0b10) !=0){
      digitalWrite(0, HIGH);
      delayMicroseconds(1500);
      digitalWrite(0, LOW);

      digitalWrite(1, HIGH);
      delayMicroseconds(1000);
      digitalWrite(1, LOW);
    
      delay(18);
    }
    else{
      digitalWrite(1, HIGH);
      delayMicroseconds(1500);
      digitalWrite(1, LOW);

      digitalWrite(0, HIGH);
      delayMicroseconds(2000);
      digitalWrite(0, LOW);
    
      delay(18);
    }

  }

  // measure light variation
  val2=val1;    // store previous measure
  val1=analogRead(1);
  val1+=analogRead(1);
  val1+=analogRead(1);
  val1+=analogRead(1);
  val1=val1>>2;  // divide by 4

  // generate enacted interaction
  if (val1<val2-2)  enacted = (intended | 0b01);  // keep action bit and set result bit to 1
  else              enacted = (intended & 0b10);  // keep action bit and set result bit to 0

  // record new interaction in timeline
  timeline = (timeline << 2);       // shift previous interactions
  timeline = (timeline | enacted);  // add enacted interaction

  // next decision
  if ( (enacted & 0b01) !=0){
    intended = ( intended ^ 0b10); // change action
  }
}
