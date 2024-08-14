#include "esp_camera.h"

// target barcode's ID
uint8_t id_target=5;

// motor speed
int speed1=250;

// pins used by the camera
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

camera_fb_t * fb = NULL;

// pins used by motors
extern int leftmotor1 =  12;
extern int leftmotor2 = 13;
extern int rightmotor1 = 15;
extern int rightmotor2 = 14;


///////////////////////////////////
void setup() {

  // start serial port
  Serial.begin(115200);

  // set motor pins as output
  pinMode(leftmotor1, OUTPUT);
  pinMode(leftmotor2, OUTPUT);
  pinMode(rightmotor1, OUTPUT);
  pinMode(rightmotor2, OUTPUT);
  digitalWrite(leftmotor1, LOW);
  digitalWrite(leftmotor2, LOW);
  digitalWrite(rightmotor1, LOW);
  digitalWrite(rightmotor2, LOW);

  ///////////////////////////////////////////////////////////
  // camera parameters
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_QVGA;
  config.pixel_format = PIXFORMAT_GRAYSCALE;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;
  ///////////////////////////////////////////////////////////

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // set image properties
  sensor_t * s = esp_camera_sensor_get();
  s->set_contrast(s, 1);
  s->set_exposure_ctrl(s, 0);
  s->set_hmirror(s, 1);        // 0 = disable , 1 = enable
  s->set_vflip(s, 1);          // 0 = disable , 1 = enable
  ///////////////////////////////////////////////////////////

  delay(1000);  // wait 1 second for components' initialization
  
}


// function to stop motors
void stop(){
      analogWrite(leftmotor1, 0);
      analogWrite(leftmotor2, 0);
      analogWrite(rightmotor1, 0);
      analogWrite(rightmotor2, 0);
}


void loop() {

  // get new image
  fb = esp_camera_fb_get();
  while (!fb){
    fb = esp_camera_fb_get();
  }

  // header for serial messages
  uint8_t start1=255;
  uint8_t start2=0;
  uint8_t start3=255;

  // variables to browse the image
  int px=0;
  uint8_t py=0;

  // pixels' values
  uint8_t pix1=0;
  uint8_t pix2=0;

  // treshold values
  uint8_t maxi=0;
  uint8_t mini=255;
  uint8_t treshold=128;

  // line scanner
  uint8_t line1=0;
  uint8_t line2=0;
  int type=0;

  // barcode position
  int p1,p2,p3,p4;    // header lines' positions
  uint8_t h1, h2, d1; // header lines' heights and spacing
  uint8_t h=0;        // header's height
  float h4=0;
  uint8_t h5=0;
  
  uint8_t b0, b1, b2, b3; // position of barcode bits in image
  uint8_t id=0;           // binary code

  // position of the barcode
  int dist=25;
  int pos=0;
  int nb_pos=0; // number of detected columns

  // send message header
  Serial.print((char)start1);
  Serial.print((char)start2);
  Serial.print((char)start3);

  // scan image column by column
  for (px=1;px<320;px++){

    // restart variable for next column
    maxi=0;
    mini=255;

    p1=-1;
    p2=-1;
    p3=-1;
    p4=-1;

    // scan column px
    for (py=1;py<238;py++){

      // get values of pixels above and below py (average of two pixel of the same line to reduce noise)
      pix1=(fb->buf[px+320*(py+1)]/2)+(fb->buf[(px-1)+320*(py+1)]/2);
      pix2=(fb->buf[px+320*(py-1)]/2)+(fb->buf[(px-1)+320*(py-1)]/2);

      // define maximum and minimum value to define treshold
      if (pix1>maxi) maxi=pix1;
      if (pix1<mini) mini=pix1;

      
      if (p1==-1){                // search line begining
        if (pix1<treshold && pix2>treshold && pix2>pix1 && pix2-pix1>30){ // detect a strong negative light gradient
          p1=py;                  // store position of line's begining
          py+=1;                  // jump next line
        }
      }
      else if (p1!=-1 && p2==-1){ // search line end
        if (pix1>treshold && pix2<treshold && pix1>pix2 && pix1-pix2>30){ // detect a strong positive light gradient
          p2=py;                  // store position of line' ending
          py+=1;
        }
      }
      

      if (p4!=-1 && p2!=-1){ // if two lines are detected
 
        h2=p2-p1; // heigh of top line
        h1=p4-p3; // heigh of bottom line
        d1=p1-p4; // spacing between lines

        if (h1<h2*2.8 && h1>h2*1.2 && h1<d1*2.8 && h1>d1*1.2){ // detect when first line is nearly twice as thick as second and spacing is nearly as thick as second line

          h=p2-p3;    // heigh of header

          // define expected position of code's bits
          b0=p3+1+(int)(h*1.35f);
          b1=p3+1+(int)(h*1.6f);
          b2=p3+1+(int)(h*1.85f);
          b3=p3+1+(int)(h*2.1f);

          if (p3>=0 && h>4 && h<100){

            // read barcodee's ID
            id=0;
            if (fb->buf[px+320*b0]<treshold) id=id | 0b00001000;
            if (fb->buf[px+320*b1]<treshold) id=id | 0b00000100;
            if (fb->buf[px+320*b2]<treshold) id=id | 0b00000010;
            if (fb->buf[px+320*b3]<treshold) id=id | 0b00000001;

            // send data on serial port
            Serial.print((char)(px/2));
            Serial.print((char)p3);
            Serial.print((char)h);
            Serial.print((char)b0);
            Serial.print((char)b1);
            Serial.print((char)b2);
            Serial.print((char)b3);
            Serial.print((char)id);

            
            if ((char)id==(char)id_target){ // if barcode has target ID

              dist+=(int)h; // integrate horizontal position and distance
              pos+=px;
              nb_pos++;
            }
          }

          py=235; // end of column scan loop
        }
      }
      
      // store detected line as second line
      if (p1!=-1 && p2!=-1){
        p3=p1;
        p4=p2;
        p1=-1;
        p2=-1;
      }
    }

    treshold=mini+((maxi-mini)/2); // get treshold for next column
  }


  if (nb_pos>3){            // control the robot if the right barcode was detected in at least three columns
    pos=pos/nb_pos;         // define horizontal position and distance
    dist=dist/nb_pos;

    // control motor according to position and distance to keep the barcode in front of the robot
    if (dist<35 && dist>10){
      // too far
      if (pos<120){       // turn left
        analogWrite(leftmotor1, 0);
        analogWrite(leftmotor2, speed1);
        analogWrite(rightmotor1, 0);
        analogWrite(rightmotor2, 0);
      }
      else if (pos>200){  // turn right
        analogWrite(leftmotor1, 0);
        analogWrite(leftmotor2, 0);
        analogWrite(rightmotor1, 0);
        analogWrite(rightmotor2, speed1);
      }
      else{               // move forward
        analogWrite(leftmotor1, 0);
        analogWrite(leftmotor2, speed1);
        analogWrite(rightmotor1, 0);
        analogWrite(rightmotor2, speed1);
      }
    }
    else if (dist>40 && dist<65){
      // too close
      if (pos<120){       // turn left
        analogWrite(leftmotor1, 0);
        analogWrite(leftmotor2, 0);
        analogWrite(rightmotor1, speed1);
        analogWrite(rightmotor2, 0);
      }
      else if (pos>200){  // turn right
        analogWrite(leftmotor1, speed1);
        analogWrite(leftmotor2, 0);
        analogWrite(rightmotor1, 0);
        analogWrite(rightmotor2, 0);
      }
      else{               // move backward
        analogWrite(leftmotor1, speed1);
        analogWrite(leftmotor2, 0);
        analogWrite(rightmotor1, speed1);
        analogWrite(rightmotor2, 0);
      }
    }
    else if (dist>=35 && dist<=40){
      // right distance
      if (pos<120){       // turn left
        analogWrite(leftmotor1, 0);
        analogWrite(leftmotor2, speed1);
        analogWrite(rightmotor1, 0);
        analogWrite(rightmotor2, 0);
      }
      else if (pos>200){  // turn right
        analogWrite(leftmotor1, 0);
        analogWrite(leftmotor2, 0);
        analogWrite(rightmotor1, 0);
        analogWrite(rightmotor2, speed1);
      }
      else{   // right position : stop the robot
        stop();
      }
    }
    else{
      // no barcode detected : stop the robot
      stop();
    }
  }
  else stop();
  
  esp_camera_fb_return(fb);
}
