#include <Servo.h>
 
 
//pendulum all the way to the right (-) is 0 degrees
//pendulum all the way to the left (+) is 180 degrees
//pendulum straight down (N) is 90 degrees
//minusnow button to move the pendulum instantly to horizontal left
//plusnow button to move the pendulum instantly to the right
//startbutton: press button to stop the program and restart
 
int pstart=0; //pendulum start position - neutral or straight down
int servoDelay = 1000; //
int inc = 10; //degrees to increment the servo - planned at 10 degrees
int pos = 0; //servo position and location of the pendulum
int startpend = 1; //flag to know striphard and no other sensor is monitored


boolean startstop = false;
 
const int behindsensor = 4;     // behindsensor connected to pin 4
const int frontsensor = 5;       //frontsensor connected to pin 5
const int striphardsensor = 6;   //striphard sensor connecte to pin 6
const int minusnowbutton = 7;   //striphard sensor connecte to pin 7
const int plusnowbutton = 8;   //striphard sensor connecte to pin 8
const int startbutton = 9;   //striphard sensor connecte to pin 9
const int servoPin = 3; //servo attached to pin 3 

Servo myPendulum; //myPendulum is the servo and pendulum name
 

 
 
void movePen(int position)
{
  Serial.print("Attempting to move pendulum to ");
  Serial.println(position);
  if (position > 180)
  {
    Serial.println("Pendulum exceeded 180, refusing to move there");
    exit;
  }
  if (position < 0)
  {
    Serial.println("Pendulum under 0, refusing to move there");
    return;
  }

  myPendulum.write(position);
  Serial.println("success");
}

void setup() {
  myPendulum.attach(servoPin); 
  pos=90; // set position variable to 90 degrees
  Serial.begin(9600);
  Serial.println("starting");
  movePen (pos); //put pendulum to 90 degrees - neutral
  pinMode(behindsensor, INPUT);
  pinMode(frontsensor, INPUT);
  pinMode(striphardsensor, INPUT);
  pinMode(minusnowbutton, INPUT);
  pinMode(plusnowbutton, INPUT);
  pinMode(startbutton, INPUT);
}
 
void loop() {
 
//scan for start button and if high then stop program and return p to 90 - startbutton
if(digitalRead(startbutton) == HIGH)  //Positive voltage to pin is HIGH
{
 pos=90; 
 movePen (pos);
 Serial.println("startbutton pressed, servo at 90");
 delay(servoDelay);
 startstop=true;
}

if((startstop) && (pos <= 180 && pos >= 0))
{
  //scan for minusnow button and if high then p=180 - minusnow
   
  if(digitalRead(minusnowbutton) == HIGH)
  {
    pos=180;
    movePen (pos);
    Serial.println("minusnow button pressed, servo at 180");
    delay(servoDelay);
    startstop=false;
  }
   
  //scan for plus button and if high then p=0 - plusnow
  if (digitalRead(plusnowbutton) == HIGH)
  {
    pos=0; 
    movePen(pos);
    Serial.println("plusnow button pressed, servo at 0");
    delay(servoDelay);
    startstop=false;
  }
   
  //start of the routine - must press striphard, and startpend =1 and pos=90
   
  if (digitalRead(striphardsensor) == LOW && pos==90)
  {
    pos=pos-inc;
    movePen (pos);
    Serial.println("striphardsenor detected, pendulum moves 10 degrees toward +");
    delay(servoDelay);
    Serial.println(pos);
  }
  
  //look for front \sensor with pendulum at 90 and go - 30 degrees
  
  if (digitalRead(frontsensor) == LOW && pos == 90)
  {
  pos=pos+(inc*3);
  movePen (pos);
  Serial.println("front sensor detected with neutral prospect, pendulum moves 30 degrees toward -");
  Serial.println(pos);
  delay(servoDelay);
  }
   
  //if behindsensor is high, pos = pos-10
   
  if (digitalRead(behindsensor) == LOW && pos!=90 && (pos < 180 && pos > 0))
  {
    pos=pos-inc;
    movePen (pos);
    Serial.println("behindsenor detected, pendulum moves 10 degrees toward +");
    Serial.println(pos);
    delay(servoDelay);
  }
   
  //if frontsensor is high, pos = pos+10
   
  if (digitalRead(frontsensor) == LOW && pos!=90 && (pos < 180 && pos > 0))
  {
    pos=pos+(inc*3);
    movePen (pos);
    Serial.println("frontsenor detected, pendulum moves 30 degrees toward -");
    Serial.println(pos);
    delay(servoDelay);
  }

//  code to be able to press start button and restart program
  if (pos == 180)
  {
    startstop=false;
  }
  if (pos == 0)
  {
    startstop = false;
  }
 }


  
}
 

 

