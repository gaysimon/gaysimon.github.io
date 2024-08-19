
// intended and enacted interactions
byte intended=0;
byte enacted=0;
byte timeline=0;      // timeline of enacted interactions

int counters[16];     // count each observed sheme
int candidates[4];    // proposition value of interactions
int candidates2[4];   // propositions with alternatives' values

byte alternative[4];  // detected alternative of interactions


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


  // if failure, record alternative interaction
  if (enacted!=intended) alternative[intended]= (enacted | 0b1000); // 4th bit indicates there is an alternative

  // increment counter of observed shemes
  counters[(timeline & 0b1111)]++;  // four last bits indicate the two last enacted interactions


  // propositions
  for (i=0;i<4;i++) candidates[i]=0;    // reset candidate values
  
  // get propositions' values
  for (i=0;i<16;i++) {
    if ( (timeline & 0b11) == (i & 0b1100)>>2) {  // if last enacted interaction is condition
      if ( (i & 0b01) == 0 )  candidates[(i & 0b11)]+=counters[i]; // valence = 1
      else                    candidates[(i & 0b11)]-=counters[i]; // valence =-1
    }
  }

  // add alternatives' values
  for (i=0;i<4;i++) {
    candidates2[i]=candidates[i];
    if ( (alternative[i] & 0b1000) !=0 ) candidates2[i]+=candidates[ (alternative[i] & 0b0011) ];
  }


  // find best proposition
  int maxVal=candidates2[0];
  intended=0;
      
  for (i=1;i<4;i++) {
    if (candidates2[i]>maxVal) {
      maxVal=candidates[2];
      intended=i;
    }
  }
}
