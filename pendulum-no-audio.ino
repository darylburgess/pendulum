#include <SoftwareSerial.h>

#include <Servo.h>
#include <IR_remote.h>
#include <EEPROM.h>

//how long to wait between moves
const long interval = 2000; //5s

//Initialize Remote
const uint16_t BUTTON_POWER = 0xD827; // i.e. 0x10EFD827
const uint16_t BUTTON_A = 0xF807;
const uint16_t BUTTON_B = 0x7887;
const uint16_t BUTTON_C = 0x58A7;
const uint16_t BUTTON_UP = 0xA05F;
const uint16_t BUTTON_DOWN = 0x00FF;
const uint16_t BUTTON_LEFT = 0x10EF;
const uint16_t BUTTON_RIGHT = 0x807F;
const uint16_t BUTTON_CIRCLE = 0x20DF;

//int RECV_PIN = 2;
const int RECV_PIN = 4;
IRrecv irrecv(RECV_PIN);
decode_results results;

//Initialize Servo
const int SERVO_PIN = 3;
Servo myservo;
int pos;

long lastmove;

const int eeAddress = 0;

int posnum = 6;

//tracks:
//  0 = default
//  1 = construction
//  2 = existing
//  3 = financial
//  4 = retail
//  5 = technology
const char filenames[][6] = 
{
  "DEFAU",
  "CONST",
  "EXIST",
  "FINAN",
  "RETAI",
  "TECHN"
};

int track = 0;

long lastmillis;



void setup()
{
  Serial.begin(9600);
  // In case the interrupt driver crashes on setup, give a clue
  // to the user what's going on.

  //wait on serial
  delay(5000);
  //Serial.println("Enabling IRin");
  irrecv.enableIRIn(); // Start the receiver
  //Serial.println("Enabled IRin");

  //servo
  myservo.attach(SERVO_PIN);

  pos = 90;

  moveServo(pos);

  lastmove = millis();
  lastmillis = millis();

}

void moveServo(int target)
{
  int current = myservo.read();
  int movepos;
  if (current < target)
  {
    for (movepos = current; movepos <= target; movepos += 1)
    {
      myservo.write(movepos);
      delay(30);
    }
  }
  else if (current > target)
  {
    for (movepos = current; movepos >= target; movepos -= 1)
    {
      myservo.write(movepos);
      delay(30);
    }
  }
}

void moveServoByNum(int position)
{
  switch (position)
  {
    case 3:
    {
      pos = 0;
    }
    break;
    case 4:
    {
      pos = 30;
    }
    break;
    case 5:
    {
      pos = 60;
    }
    break;
    case 6:
    {
      pos = 90;
    }
    break;
    case 7:
    {
      pos = 120;
    }
    break;
    case 8:
    {
      pos = 150;
    }
    break;
    case 9:
    {
      pos = 180;
    }
    break;
  }
  moveServo(pos);
}


void loop() {

  if ((millis() - interval) > lastmillis)
  {
    lastmillis = millis();
  }

  
  if (irrecv.decode(&results)) 
  {
    /* read the RX'd IR into a 16-bit variable: */
    uint16_t resultCode = (results.value & 0xFFFF);

    // This switch statement checks the received IR code against
    // all of the known codes. Each button press produces a 
    // serial output, and has an effect on the LED output.
    switch (resultCode)
    {
      case BUTTON_LEFT:
        Serial.println("Left");
        //move pendulum to left
        if (posnum < 9)
        {
          posnum++;
        }
	moveServoByNum(posnum);
        break;
      case BUTTON_RIGHT:
        Serial.println("Right");
        if (posnum > 3)
        {
          posnum--;
        }
	moveServoByNum(posnum);
        break;
      case BUTTON_CIRCLE:
        Serial.println("Circle");
        break;
      case BUTTON_A:
        Serial.println("A");
        posnum = 9;
        moveServoByNum(posnum);
        break;
      case BUTTON_B:
        Serial.println("B");
        posnum = 6;
        moveServoByNum(posnum);
        break;
      case BUTTON_C:
        Serial.println("C");
        posnum = 3;
        moveServoByNum(posnum);
        break;
      default:
        //Serial.print("Unrecognized code received: 0x");
        //Serial.println(results.value, HEX);
        break;        
    }    
    irrecv.resume(); // Receive the next value
  }
  delay(100);
}
