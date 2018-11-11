#include <SoftwareSerial.h>
#include <Adafruit_Soundboard.h>

#include <Servo.h>
#include <IR_remote.h>
#include <EEPROM.h>

//how long to wait between moves
const long interval = 2000; //5s

//Initialize Sound Board
#define SFX_TX 5
#define SFX_RX 6
#define SFX_RST 4

SoftwareSerial ss = SoftwareSerial(SFX_TX, SFX_RX);
Adafruit_Soundboard sfx = Adafruit_Soundboard(&ss, NULL, SFX_RST);


//Initialize Sensor Pins

#define SENS35 7
#define SENS45 8
#define SENS55 9
#define SENS65 10
#define SENS75 11
#define SENS85 12

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

//New Remote Values
const uint16_t NEW_POWER = 0x629D;
const uint16_t NEW_A = 0x22DD;
const uint16_t NEW_B = 0x02FD;
const uint16_t NEW_C = 0xC23D;
const uint16_t NEW_UP = 0x9867;
const uint16_t NEW_DOWN = 0x38C7;
const uint16_t NEW_LEFT = 0x30CF;
const uint16_t NEW_RIGHT = 0x7A85;
const uint16_t NEW_CIRCLE = 0x18E7;

//int RECV_PIN = 2;
const int RECV_PIN = 14;
IRrecv irrecv(RECV_PIN);
decode_results results;
//uint16_t lastCode = 0; // This keeps track of the last code RX'd


//Initialize Servo
const int SERVO_PIN = 3;
Servo myservo;
int pos;

long lastmove;

const int eeAddress = 0;

int posnum = 6;

int program = 3;
int prevprogram = 3;
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

//variables for program C
bool movingleft = false;
long penlastmillis;
const int pendelay = 30;


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

  pinMode(SENS35, INPUT);
  pinMode(SENS45, INPUT);
  pinMode(SENS55, INPUT);
  pinMode(SENS65, INPUT);
  pinMode(SENS75, INPUT);
  pinMode(SENS85, INPUT);

  //servo
  myservo.attach(SERVO_PIN);

  ss.begin(9600);
 
    
  EEPROM.get(eeAddress, track);
  if ((track > 5) || (track < 0))
  {
    track = 0;
  }

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

void initB()
{
  char trackname[20];
  int newposnum = posnum;
  while (newposnum == posnum)
  {
    newposnum = random(3, 9);
  }
  posnum = newposnum;
  Serial.print("I have chosen position ");
  Serial.print(posnum);
  Serial.println(" at random");
  //play audio
  switch (posnum)
  {
    case 3:
      sprintf(trackname, "%s%s%s", "T03", filenames[track], "OGG\n");
      break;
    case 4:
      sprintf(trackname, "%s%s%s", "T04", filenames[track], "OGG\n");
      break;
    case 5:
      sprintf(trackname, "%s%s%s", "T05", filenames[track], "OGG\n");
      break;
    case 6:
      sprintf(trackname, "%s%s%s", "T06", filenames[track], "OGG\n");
      break;
    case 7:
      sprintf(trackname, "%s%s%s", "T07", filenames[track], "OGG\n");
      break;
    case 8:
      sprintf(trackname, "%s%s%s", "T08", filenames[track], "OGG\n");
      break;
    case 9:
      sprintf(trackname, "%s%s%s", "T09", filenames[track], "OGG\n");
      break;
  }
  Serial.println(trackname);
  sfx.playTrack(trackname);
}

void loop() {

  char trackname[20];

  if ((millis() - interval) > lastmillis)
  {
    lastmillis = millis();
  }

  if (program == 1)
  {
    switch (posnum)
    {
      case 3:
        //position 3
	//do nothing - you win!
      break;
      case 4:
        //position 4
	//see if any targets hit
	//if 4.5 hit, move to 3
	//otherwise move to 5
	if (digitalRead(SENS45)==HIGH)
	{
	  posnum = 3;
	  //move servo
	  moveServoByNum(posnum);
	  //play T03 sound
          sprintf(trackname, "%s%s%s", "T03", filenames[track], "OGG\n");
	  Serial.println(trackname);
          sfx.playTrack(trackname);
	}
	else if (digitalRead(SENS35)==HIGH || digitalRead(SENS55)==HIGH || digitalRead(SENS65)==HIGH || digitalRead(SENS75)==HIGH || digitalRead(SENS85)==HIGH)
	{
	  posnum = 5;
	  //move servo
	  moveServoByNum(posnum);
	  //play T05 sound
          sprintf(trackname, "%s%s%s", "T05", filenames[track], "OGG\n");
	  Serial.println(trackname);
          sfx.playTrack(trackname);
	}
      break;
      case 5:
        //position 5
	//see if any targets hit
	//if 5.5 hit, move to 4
	//otherwise move to 6
	if (digitalRead(SENS55)==HIGH)
	{
	  posnum = 4;
	  //move servo
	  moveServoByNum(posnum);
	  //play T04 sound
          sprintf(trackname, "%s%s%s", "T04", filenames[track], "OGG\n");
	  Serial.println(trackname);
          sfx.playTrack(trackname);
	}
	else if (digitalRead(SENS35)==HIGH || digitalRead(SENS45)==HIGH || digitalRead(SENS65)==HIGH || digitalRead(SENS75)==HIGH || digitalRead(SENS85)==HIGH)
	{
	  posnum = 6;
	  //move servo
	  moveServoByNum(posnum);
	  //play T06 sound
          sprintf(trackname, "%s%s%s", "T06", filenames[track], "OGG\n");
	  Serial.println(trackname);
          sfx.playTrack(trackname);
	}
      break;
      case 6:
        //position 6
	//see if any targets hit
	//if 7.5 hit, move to 5
	//otherwise move to 7
	if (digitalRead(SENS75)==HIGH)
	{
	  posnum = 5;
	  //move servo
	  moveServoByNum(posnum);
	  //play T05 sound
          sprintf(trackname, "%s%s%s", "T05", filenames[track], "OGG\n");
	  Serial.println(trackname);
          sfx.playTrack(trackname);
	}
	else if (digitalRead(SENS35)==HIGH || digitalRead(SENS45)==HIGH || digitalRead(SENS55)==HIGH || digitalRead(SENS65)==HIGH || digitalRead(SENS85)==HIGH)
	{
	  posnum = 7;
	  //move servo
	  moveServoByNum(posnum);
	  //play T07 sound
          sprintf(trackname, "%s%s%s", "T07", filenames[track], "OGG\n");
	  Serial.println(trackname);
          sfx.playTrack(trackname);
	}
      break;
      case 7:
        //position 7
	//see if any targets hit
	//if 7.5 hit, move to 6
	//otherwise move to 8
	if (digitalRead(SENS75)==HIGH)
	{
	  posnum = 6;
	  //move servo
	  moveServoByNum(posnum);
	  //play T06 sound
          sprintf(trackname, "%s%s%s", "T06", filenames[track], "OGG\n");
	  Serial.println(trackname);
          sfx.playTrack(trackname);
	}
	else if (digitalRead(SENS35)==HIGH || digitalRead(SENS45)==HIGH || digitalRead(SENS55)==HIGH || digitalRead(SENS65)==HIGH || digitalRead(SENS85)==HIGH)
	{
	  posnum = 8;
	  //move servo
	  moveServoByNum(posnum);
	  //play T08 sound
          sprintf(trackname, "%s%s%s", "T08", filenames[track], "OGG\n");
	  Serial.println(trackname);
          sfx.playTrack(trackname);
	}
      break;
      case 8:
        //position 8
	//see if any targets hit
	//if 8.5 hit, move to 7
	//otherwise move to 9
	if (digitalRead(SENS85)==HIGH)
	{
	  posnum = 7;
	  //move servo
	  moveServoByNum(posnum);
	  //play T07 sound
          sprintf(trackname, "%s%s%s", "T07", filenames[track], "OGG\n");
	  Serial.println(trackname);
          sfx.playTrack(trackname);
	}
	else if (digitalRead(SENS35)==HIGH || digitalRead(SENS45)==HIGH || digitalRead(SENS55)==HIGH || digitalRead(SENS65)==HIGH || digitalRead(SENS75)==HIGH)
	{
	  posnum = 9;
	  //move servo
	  moveServoByNum(posnum);
	  //play T09 sound
          sprintf(trackname, "%s%s%s", "T09", filenames[track], "OGG\n");
	  Serial.println(trackname);
          sfx.playTrack(trackname);
	}
      break;
      case 9:
        //position 9
	//do nothing - you lose :(
      break;
    }

  }

  if (program == 2)
  {
      if ((millis() - pendelay) > penlastmillis)
      {
        if (movingleft)
	{
	  if (pos < 180)
          {
	    pos++;
	  }
	  else
	  {
	    movingleft = false;
	  }
	  myservo.write(pos);
	}
	else
	{
	  if (pos > 0)
	  {
	    pos--;
	  }
	  else
	  {
            movingleft = true;
	  }
	  myservo.write(pos);
	}
      }
      
  }

  
  if (irrecv.decode(&results)) 
  {
    /* read the RX'd IR into a 16-bit variable: */
    uint16_t resultCode = (results.value & 0xFFFF);
    Serial.println(resultCode);

    // This switch statement checks the received IR code against
    // all of the known codes. Each button press produces a 
    // serial output, and has an effect on the LED output.
    if ((resultCode == BUTTON_POWER) || (resultCode == NEW_POWER))
    {
        Serial.println("Power");
        //select which program
        Serial.println("Mode select enabled");
        //call audio file "please select program"
        sprintf(trackname, "%s", "T00PROGSOGG\n");
        Serial.println(trackname);
        sfx.playTrack(trackname);
        prevprogram = program;
        program = 4;
    }
    if ((resultCode == BUTTON_A) || (resultCode == NEW_A))
    {
        Serial.println("A");
        switch (program)
        {
          case 1: //program A - interact with pendulum
            posnum = 9;
	    moveServoByNum(posnum);
            break;
          case 2: //program B - guess position
            break;
          case 3: //program C - instructor mode
            //move pendulum to far left
            posnum = 9;
	    moveServoByNum(posnum);
            break;
          case 4: //program select mode
            sprintf(trackname, "%s", "T00PSELAOGG\n");
	    Serial.println(trackname);
            sfx.playTrack(trackname);
            delay(3000);
            program = 1;
	    pos = 90;
	    posnum = 6;
	    moveServoByNum(posnum);
	    //play T06 sound
            sprintf(trackname, "%s%s%s", "T06", filenames[track], "OGG\n");
	    Serial.println(trackname);
            sfx.playTrack(trackname);
            break;
        }
    }
    if ((resultCode == BUTTON_B) || (resultCode == NEW_B))
    {
        Serial.println("B");
        switch (program)
        {
          case 1: //program A - interact with pendulum
            posnum = 6;
	    moveServoByNum(posnum);
            break;
          case 2: //program B - guess position
            break;
          case 3: //program C - instructor mode
            //move pendulum to bottom (90)
            posnum = 6;
	    moveServoByNum(posnum);
            break;
          case 4: //program select mode
            sprintf(trackname, "%s", "T00PSELBOGG\n");
	    Serial.println(trackname);
            sfx.playTrack(trackname);
            delay(3000);
            program = 2;
            penlastmillis = millis();
            initB();
            break;
        }
    }
    if ((resultCode == BUTTON_C) || (resultCode == NEW_C))
    {
        Serial.println("C");
        switch (program)
        {
          case 1: //program A - interact with pendulum
            posnum = 3;
	    moveServoByNum(posnum);
            break;
          case 2: //program B - guess position
            break;
          case 3: //program C - instructor mode
            //move pendulum to 3
            posnum = 3;
	    moveServoByNum(posnum);
            break;
          case 4: //program select mode
            sprintf(trackname, "%s", "T00PSELCOGG\n");
	    Serial.println(trackname);
            sfx.playTrack(trackname);
            delay(3000);
            program = 3;
            break;
        }
    }
    if ((resultCode == BUTTON_UP) || (resultCode == NEW_UP))
    {
        Serial.println("Up");
        switch (program)
        {
          case 1: //program A - interact with pendulum
            //sprintf(trackname, "%s%s%s%s", "T0", posnum, filenames[track], "OGG\n");
            switch (posnum)
            {
              case 3:
                sprintf(trackname, "%s%s%s", "T03", filenames[track], "OGG\n");
                break;
              case 4:
                sprintf(trackname, "%s%s%s", "T04", filenames[track], "OGG\n");
                break;
              case 5:
                sprintf(trackname, "%s%s%s", "T05", filenames[track], "OGG\n");
                break;
              case 6:
                sprintf(trackname, "%s%s%s", "T06", filenames[track], "OGG\n");
                break;
              case 7:
                sprintf(trackname, "%s%s%s", "T07", filenames[track], "OGG\n");
                break;
              case 8:
                sprintf(trackname, "%s%s%s", "T08", filenames[track], "OGG\n");
                break;
              case 9:
                sprintf(trackname, "%s%s%s", "T09", filenames[track], "OGG\n");
                break;
            }
	    Serial.println(trackname);
            sfx.playTrack(trackname);
            break;
          case 2: //program B - guess position
            //play the sound again for the user
            switch (posnum)
            {
              case 3:
                sprintf(trackname, "%s%s%s", "T03", filenames[track], "OGG\n");
                break;
              case 4:
                sprintf(trackname, "%s%s%s", "T04", filenames[track], "OGG\n");
                break;
              case 5:
                sprintf(trackname, "%s%s%s", "T05", filenames[track], "OGG\n");
                break;
              case 6:
                sprintf(trackname, "%s%s%s", "T06", filenames[track], "OGG\n");
                break;
              case 7:
                sprintf(trackname, "%s%s%s", "T07", filenames[track], "OGG\n");
                break;
              case 8:
                sprintf(trackname, "%s%s%s", "T08", filenames[track], "OGG\n");
                break;
              case 9:
                sprintf(trackname, "%s%s%s", "T09", filenames[track], "OGG\n");
                break;
            }
	    Serial.println(trackname);
            sfx.playTrack(trackname);
            break;
          case 3: //program C - instructor mode
            switch (posnum)
            {
              case 3:
                sprintf(trackname, "%s%s%s", "T03", filenames[track], "OGG\n");
                break;
              case 4:
                sprintf(trackname, "%s%s%s", "T04", filenames[track], "OGG\n");
                break;
              case 5:
                sprintf(trackname, "%s%s%s", "T05", filenames[track], "OGG\n");
                break;
              case 6:
                sprintf(trackname, "%s%s%s", "T06", filenames[track], "OGG\n");
                break;
              case 7:
                sprintf(trackname, "%s%s%s", "T07", filenames[track], "OGG\n");
                break;
              case 8:
                sprintf(trackname, "%s%s%s", "T08", filenames[track], "OGG\n");
                break;
              case 9:
                sprintf(trackname, "%s%s%s", "T09", filenames[track], "OGG\n");
                break;
            }
	    Serial.println(trackname);
            sfx.playTrack(trackname);
            break;
          case 4: //program select mode
            if (track < 5)
            {
              track++;
              sprintf(trackname, "%s%s%s", "T00", filenames[track], "OGG\n");
	      Serial.println(trackname);
              sfx.playTrack(trackname);
              EEPROM.put(eeAddress, track);
            }
            break;
        }
    }
    if ((resultCode == BUTTON_DOWN) || (resultCode == NEW_DOWN))
    {
        Serial.println("Down");
        switch (program)
        {
          case 1: //program A - interact with pendulum
            break;
          case 2: //program B - guess position
            break;
          case 3: //program C - instructor mode
            break;
          case 4: //program select mode
	    if (track > 0)
            {
              track--;
              sprintf(trackname, "%s%s%s", "T00", filenames[track], "OGG\n");
	      Serial.println(trackname);
              sfx.playTrack(trackname);
              EEPROM.put(eeAddress, track);
            }
            break;
        }
    }
    if ((resultCode == BUTTON_LEFT) || (resultCode == NEW_LEFT))
    {
        Serial.println("Left");
        switch (program)
        {
          case 1: //program A - interact with pendulum
            //move pendulum to left
            if (posnum < 9)
            {
              posnum++;
            }
	    moveServoByNum(posnum);
            break;
          case 2: //program B - guess position
            break;
          case 3: //program C - instructor mode
            //move pendulum to left
            if (posnum < 9)
            {
              posnum++;
            }
	    moveServoByNum(posnum);
            break;
          case 4: //program select mode
            break;
        }
    }
    if ((resultCode == BUTTON_RIGHT) || (resultCode == NEW_RIGHT))
    {
        Serial.println("Right");
        switch (program)
        {
          case 1: //program A - interact with pendulum
            //move pendulum to right
            if (posnum > 3)
            {
              posnum--;
            }
	    moveServoByNum(posnum);
            break;
          case 2: //program B - guess position
            break;
          case 3: //program C - instructor mode
            //move pendulum to right
            if (posnum > 3)
            {
              posnum--;
            }
	    moveServoByNum(posnum);
            break;
          case 4: //program select mode
            break;
        }
    }
    if ((resultCode == BUTTON_CIRCLE) || (resultCode == NEW_CIRCLE))
    {
        Serial.println("Circle");
        switch (program)
        {
          case 1: //program A - interact with pendulum
            break;
          case 2: //program B - guess position
            //see if pendulum is in the "correct" position
            switch (posnum)
            {
              case 3:
                if (pos < 15)
                {
                  sprintf(trackname, "%s", "T00DINGGOGG\n");
                  Serial.println(trackname);
                  sfx.playTrack(trackname);
                  //pick new position
                  delay(5000);
                  initB();
                }
                else
                {
                  sprintf(trackname, "%s", "T00URRRGOGG\n");
                  Serial.println(trackname);
                  sfx.playTrack(trackname);
                }
                break;
              case 4:
                if ((pos < 45) && (pos > 15))
                {
                  sprintf(trackname, "%s", "T00DINGGOGG\n");
                  Serial.println(trackname);
                  sfx.playTrack(trackname);
                  //pick new position
                  delay(5000);
                  initB();
                }
                else
                {
                  sprintf(trackname, "%s", "T00URRRGOGG\n");
                  Serial.println(trackname);
                  sfx.playTrack(trackname);
                }
                break;
              case 5:
                if ((pos < 75) && (pos > 45))
                {
                  sprintf(trackname, "%s", "T00DINGGOGG\n");
                  Serial.println(trackname);
                  sfx.playTrack(trackname);
                  //pick new position
                  delay(5000);
                  initB();
                }
                else
                {
                  sprintf(trackname, "%s", "T00URRRGOGG\n");
                  Serial.println(trackname);
                  sfx.playTrack(trackname);
                }
                break;
              case 6:
                if ((pos < 105) && (pos > 75))
                {
                  sprintf(trackname, "%s", "T00DINGGOGG\n");
                  Serial.println(trackname);
                  sfx.playTrack(trackname);
                  //pick new position
                  delay(5000);
                  initB();
                }
                else
                {
                  sprintf(trackname, "%s", "T00URRRGOGG\n");
                  Serial.println(trackname);
                  sfx.playTrack(trackname);
                }
                break;
              case 7:
                if ((pos < 135) && (pos > 105))
                {
                  sprintf(trackname, "%s", "T00DINGGOGG\n");
                  Serial.println(trackname);
                  sfx.playTrack(trackname);
                  //pick new position
                  delay(5000);
                  initB();
                }
                else
                {
                  sprintf(trackname, "%s", "T00URRRGOGG\n");
                  Serial.println(trackname);
                  sfx.playTrack(trackname);
                }
                break;
              case 8:
                if ((pos < 165) && (pos > 135))
                {
                  sprintf(trackname, "%s", "T00DINGGOGG\n");
                  Serial.println(trackname);
                  sfx.playTrack(trackname);
                  //pick new position
                  delay(5000);
                  initB();
                }
                else
                {
                  sprintf(trackname, "%s", "T00URRRGOGG\n");
                  Serial.println(trackname);
                  sfx.playTrack(trackname);
                }
                break;
            }
            break;
          case 3: //program C - instructor mode
            break;
          case 4: //program select mode
            break;
        }
    }
    irrecv.resume(); // Receive the next value
  }
  delay(100);
}
