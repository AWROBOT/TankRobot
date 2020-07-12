#include <Servo.h>
#include <IRremote.h>
#include <TimerFreeTone.h>

#include "Sounds.h"

#define PIN_Servo   10
#define PIN_Trigger 11
#define PIN_Echo    12
#define PIN_IR      18
#define PIN_Buzzer  19

//WHITE LEDS
#define PIN_LED_1   14
#define PIN_LED_2   15
#define PIN_LED_3   16
#define PIN_LED_4   17

//RED LEDS
#define PIN_LED_5   2
#define PIN_LED_6   3
#define PIN_LED_7   4
#define PIN_LED_8   5

//MOTOR PINS
#define PIN_MOTOR_1 6
#define PIN_MOTOR_2 7
#define PIN_MOTOR_3 8
#define PIN_MOTOR_4 9

IRrecv irrecv(PIN_IR);
decode_results results;

Servo servo_head;
int servo_value = 70;

int distanceCounter = 0;
int numcycles = 0;  // Number of cycles used to rotate with head during moving
int roam = 0;       // Switching between automatic and manual mode of moving
int animation = 0;  // Led animation

// There are limits for obstacles:
const int distanceLimit = 30;           // Front distance limit in cm (27)
const int sideDistanceLimit = 40;       // Side distance limit in cm (12)
const int turnTime = 300;               // Time needed to turn robot

char choice;
char turnDirection;  // Gets 'l', 'r' or 'f' depending on which direction is obstacle free

unsigned int duration;
unsigned int distance;
unsigned int FrontDistance;
unsigned int LeftDistance;
unsigned int RightDistance;
unsigned int LeftDiagonalDistance;
unsigned int RightDiagonalDistance;
unsigned int Time;
unsigned int adDistance;
unsigned int CollisionCounter;

int allLedArray[] = {PIN_LED_1, PIN_LED_2, PIN_LED_3, PIN_LED_4, PIN_LED_5, PIN_LED_6, PIN_LED_7, PIN_LED_8};
int count = 0;
int timer = 30;

void setup() 
{
  Serial.begin(9600);

  //WHITE LEDS
  pinMode(PIN_LED_1, OUTPUT);
  pinMode(PIN_LED_2, OUTPUT);
  pinMode(PIN_LED_3, OUTPUT);
  pinMode(PIN_LED_4, OUTPUT);

  //RED LEDS
  pinMode(PIN_LED_5, OUTPUT);
  pinMode(PIN_LED_6, OUTPUT);
  pinMode(PIN_LED_7, OUTPUT);
  pinMode(PIN_LED_8, OUTPUT);

  //MOTOR PINS
  pinMode(PIN_MOTOR_1, OUTPUT);
  pinMode(PIN_MOTOR_2, OUTPUT);
  pinMode(PIN_MOTOR_3, OUTPUT);
  pinMode(PIN_MOTOR_4, OUTPUT);

  //SERVO
  servo_head.attach(PIN_Servo); 
  servo_head.write(70); //Default

  //US
  pinMode(PIN_Trigger, OUTPUT);
  pinMode(PIN_Echo, INPUT);

  //IR
  irrecv.enableIRIn();

  moveStop();

  sing(S_connection);
}

void loop() 
{
  if (irrecv.decode(&results))
  { 
    Serial.println (results.value, HEX); //display HEX
    decodeIR();
    delay(500);
    irrecv.resume(); //next value
  }

  if(roam == 1)
  {
    go();
  }

  if(animation == 1)
  {
    ledAnimation();
  }
}

void ledAnimation()
{
  for (count=0;count<7;count++) {
   digitalWrite(allLedArray[count], HIGH);
   delay(timer);
   digitalWrite(allLedArray[count + 1], HIGH);
   delay(timer);
   digitalWrite(allLedArray[count], LOW);
   delay(timer*2);
  }
  for (count=7;count>0;count--) {
   digitalWrite(allLedArray[count], HIGH);
   delay(timer);
   digitalWrite(allLedArray[count - 1], HIGH);
   delay(timer);
   digitalWrite(allLedArray[count], LOW);
   delay(timer*2);
  }
}

void clearLedAnimation()
{
  if(animation == 1)
  {
    digitalWrite(PIN_LED_1, LOW);   
    digitalWrite(PIN_LED_2, LOW);
    digitalWrite(PIN_LED_3, LOW);
    digitalWrite(PIN_LED_4, LOW);
    digitalWrite(PIN_LED_5, LOW);   
    digitalWrite(PIN_LED_6, LOW);
    digitalWrite(PIN_LED_7, LOW);
    digitalWrite(PIN_LED_8, LOW); 
  }
  
  animation = 0;
}

//If IR signal is received than decode it and decide what to do
void decodeIR()
{
  switch(results.value)
  {
    case 0xFF18E7:  
    Serial.println("Up Arrow"); 
    roam = 0;
    clearLedAnimation(); 
    moveForward(); 
    break;

    case 0xFF10EF:  
    Serial.println("Left Arrow"); 
    roam = 0;
    clearLedAnimation(); 
    moveLeft(); 
    delay(200);
    moveStop();
    break;

    case 0xFF38C7:  
    Serial.println("OK"); 
    roam = 0;
    clearLedAnimation(); 
    moveStop();
    break;

    case 0xFF5AA5:  
    Serial.println("Right Arrow"); 
    roam = 0;
    clearLedAnimation();  
    moveRight();  
    delay(200);
    moveStop(); 
    break;

    case 0xFF4AB5:  
    Serial.println("Down Arrow"); 
    roam = 0;
    clearLedAnimation(); 
    moveBackward();
    break;

    case 0xFFA25D:  
    Serial.println("1"); 
    clearLedAnimation(); 
    digitalWrite(PIN_LED_1, HIGH);   
    digitalWrite(PIN_LED_2, HIGH);
    digitalWrite(PIN_LED_3, HIGH);
    digitalWrite(PIN_LED_4, HIGH);
    break;

    case 0xFF629D:  
    Serial.println("2"); 
    clearLedAnimation(); 
    digitalWrite(PIN_LED_1, LOW);   
    digitalWrite(PIN_LED_2, LOW);
    digitalWrite(PIN_LED_3, LOW);
    digitalWrite(PIN_LED_4, LOW);
    break;

    case 0xFFE21D:  
    Serial.println("3"); 
    clearLedAnimation(); 
    digitalWrite(PIN_LED_5, HIGH);   
    digitalWrite(PIN_LED_6, HIGH);
    digitalWrite(PIN_LED_7, HIGH);
    digitalWrite(PIN_LED_8, HIGH);
    break;

    case 0xFF22DD:  
    Serial.println("4");
    clearLedAnimation(); 
    digitalWrite(PIN_LED_5, LOW);   
    digitalWrite(PIN_LED_6, LOW);
    digitalWrite(PIN_LED_7, LOW);
    digitalWrite(PIN_LED_8, LOW); 
    break;

    case 0xFF02FD:  
    Serial.println("5");
    clearLedAnimation(); 
    animation = 1; 
    break;

    case 0xFFC23D:  
    Serial.println("6");
    clearLedAnimation(); 
    break;

    case 0xFFE01F:  
    Serial.println("7"); 
    clearLedAnimation(); 
    servo_value += 20;
    if(servo_value > 130)
      servo_value = 130;
    servo_head.write(servo_value);
    break;

    case 0xFFA857:  
    Serial.println("8"); 
    clearLedAnimation(); 
    servo_value = 70;
    servo_head.write(servo_value);
    break;

    case 0xFF906F:  
    Serial.println("9"); 
    clearLedAnimation(); 
    servo_value -= 20;
    if(servo_value < 10)
      servo_value = 10;
    servo_head.write(servo_value);
    break;

    case 0xFF6897:  
    Serial.println("*"); 
    moveStop();
    roam = 1; 
    clearLedAnimation(); 
    break;

    case 0xFF9867:  
    Serial.println("0"); 
    clearLedAnimation(); 
    _sing(random(1, 20));
    break;

    case 0xFFB04F:  
    Serial.println("#"); 
    moveStop();
    roam = 0; 
    clearLedAnimation(); 
    break; 

    default:
    Serial.println(" Unknown code received");
    break;
  }
}

///////////////////////////////////////////////////////////////////
//-- ROAM FUNCTIONS ---------------------------------------------//
///////////////////////////////////////////////////////////////////

//This function toggle between autonomous and stop mode
void toggleRoam()
{                                    
  if(roam == 0)
  {
    roam = 1;
    Serial.println("Activated Roam Mode");
  }
  else
  {
    roam = 0;
    moveStop();
    Serial.println("De-activated Roam Mode");
  }
}

void go() 
{
  moveForward();
  
  ++numcycles;
  
  if(numcycles > 40)  // After 40 cycles of code measure surrounding obstacles
  {
    Serial.println("Front obstancle detected");

    sing(S_surprise);
    
    watchsurrounding();
    if( LeftDistance < sideDistanceLimit || LeftDiagonalDistance < sideDistanceLimit)
    {
      Serial.println("Moving: RIGHT");

      sing(S_happy_short);
      
      moveRight();
      delay(turnTime);
    }
    if( RightDistance < sideDistanceLimit || RightDiagonalDistance < sideDistanceLimit)
    {
      Serial.println("Moving: LEFT");

      sing(S_happy_short);
      
      moveLeft();
      delay(turnTime);
    }
    numcycles=0; //Restart count of cycles
  }
  
  scan();
  
  if( distance < distanceLimit)
  {
    distanceCounter++;
  }
  
  if( distance > distanceLimit)
  {
    distanceCounter = 0;
  }
  
  if(distanceCounter > 7)    // robot reachaed 7 times distance limit in front of the robot, so robot must stop immediately and decide right way
  {
    moveStop();
    turnDirection = decide();
     switch (turnDirection){
      case 'l':
        moveLeft();
        delay(turnTime);
        break;
      case 'r':
        moveRight();
        delay(turnTime);
        break;
      case 'b':
        moveBackward();
        delay(turnTime);
        break;
      case 'f':
        break;
    }
    distanceCounter = 0;
  }
}

//This function determines the distance things are away from the ultrasonic sensor
int scan()                                         
{
  long pulse;
  Serial.println("Scanning distance");
  digitalWrite(PIN_Trigger,LOW);
  delayMicroseconds(5);                                                                              
  digitalWrite(PIN_Trigger,HIGH);
  delayMicroseconds(15);
  digitalWrite(PIN_Trigger,LOW);
  pulse = pulseIn(PIN_Echo,HIGH);
  distance = round( pulse*0.01657 );
  Serial.println(distance);
}

void watchsurrounding()
{ 
  //Meassures distances to the right, left, front, left diagonal, right diagonal and asign them in cm to the variables rightscanval, 
  //leftscanval, centerscanval, ldiagonalscanval and rdiagonalscanval (there are 5 points for distance testing)
  scan();
  FrontDistance = distance;
  Serial.println("Front distance measuring done");
  if(FrontDistance < distanceLimit) 
  {
    moveStop;
  }
  servo_head.write(102);
  delay(100);
  scan();
  LeftDiagonalDistance = distance;
  Serial.println("Left diagonal distance measuring done");
  if(LeftDiagonalDistance < distanceLimit)
  {
    moveStop();
  }
  servo_head.write(134);
  delay(300);
  scan();
  LeftDistance = distance;
  Serial.println("Left distance measuring done");
  if(LeftDistance < sideDistanceLimit)
  {
    moveStop();
  }
  servo_head.write(102);
  delay(100);
  scan();
  LeftDiagonalDistance = distance;
  Serial.println("Left diagonal distance measuring done");
  if(LeftDiagonalDistance < distanceLimit)
  {
    moveStop();
  }
  servo_head.write(70);
  delay(100);
  scan();
  FrontDistance = distance;
  Serial.println("Front distance measuring done");
  if(FrontDistance < distanceLimit)
  {
    moveStop();
  }
  servo_head.write(38);
  delay(100);
  scan();
  RightDiagonalDistance = distance;
  Serial.println("Right diagonal distance measuring done");
  if(RightDiagonalDistance < distanceLimit)
  {
    moveStop();
  }
  servo_head.write(6);
  delay(100);
  scan();
  RightDistance = distance;
  Serial.println("Right distance measuring done");
  if(RightDistance < sideDistanceLimit)
  {
    moveStop();
  }
 
  servo_head.write(70); //Finish looking around (look forward again)
  delay(300);
  Serial.println("Measuring done");
}

char decide()
{
   // Decide the right way without obstacles
  watchsurrounding();
  if (LeftDistance > RightDistance && LeftDistance > FrontDistance){
    Serial.println("Choise result is: LEFT");
    choice = 'l';
  }
  else if (RightDistance > LeftDistance && RightDistance > FrontDistance){
    Serial.println("Choise result is: RIGHT");
    choice = 'r';
  }
  else if ( LeftDistance < sideDistanceLimit && RightDistance < sideDistanceLimit && FrontDistance < distanceLimit ) {
    Serial.println("Choice result is: BACK"); 
    choice = 'b';
  }
  else{
    Serial.println("Choise result is: FORWARD");
    choice = 'f';
  }
  return choice;
}

///////////////////////////////////////////////////////////////////
//-- MOTION FUNCTIONS -------------------------------------------//
///////////////////////////////////////////////////////////////////

void moveForward()                                  
{
  Serial.println("moveForward");

  digitalWrite(PIN_MOTOR_1, HIGH);   
  digitalWrite(PIN_MOTOR_2, LOW);
  digitalWrite(PIN_MOTOR_3, HIGH);   
  digitalWrite(PIN_MOTOR_4, LOW);
}

void moveBackward()                                  
{
  Serial.println("moveBackward");

  digitalWrite(PIN_MOTOR_1, LOW);   
  digitalWrite(PIN_MOTOR_2, HIGH);
  digitalWrite(PIN_MOTOR_3, LOW);   
  digitalWrite(PIN_MOTOR_4, HIGH);
}

void moveRight()                                     
{
  Serial.println("moveRight");

  digitalWrite(PIN_MOTOR_1, LOW);   
  digitalWrite(PIN_MOTOR_2, HIGH);
  digitalWrite(PIN_MOTOR_3, HIGH);   
  digitalWrite(PIN_MOTOR_4, LOW);
}

void moveLeft()                                 
{
  Serial.println("moveLeft");

  digitalWrite(PIN_MOTOR_1, HIGH);   
  digitalWrite(PIN_MOTOR_2, LOW);
  digitalWrite(PIN_MOTOR_3, LOW);   
  digitalWrite(PIN_MOTOR_4, HIGH);
}

void moveStop()                                    
{
  Serial.println("moveStop");

  digitalWrite(PIN_MOTOR_1, LOW);   
  digitalWrite(PIN_MOTOR_2, LOW);
  digitalWrite(PIN_MOTOR_3, LOW);   
  digitalWrite(PIN_MOTOR_4, LOW);
}

///////////////////////////////////////////////////////////////////
//-- SOUNDS -----------------------------------------------------//
///////////////////////////////////////////////////////////////////

void _tone (float noteFrequency, long noteDuration, int silentDuration){

    // tone(10,261,500);
    // delay(500);

      if(silentDuration==0){silentDuration=1;}

      TimerFreeTone(PIN_Buzzer, noteFrequency, noteDuration);
      delay(noteDuration);       //milliseconds to microseconds
      //noTone(PIN_Buzzer);
      delay(silentDuration);     
}


void bendTones (float initFrequency, float finalFrequency, float prop, long noteDuration, int silentDuration)
{
  //Examples:
  //  bendTones (880, 2093, 1.02, 18, 1);
  //  bendTones (note_A5, note_C7, 1.02, 18, 0);

  if(silentDuration==0){silentDuration=1;}

  if(initFrequency < finalFrequency)
  {
      for (int i=initFrequency; i<finalFrequency; i=i*prop) 
      {
          _tone(i, noteDuration, silentDuration);
      }
  } 
  else
  {
      for (int i=initFrequency; i>finalFrequency; i=i/prop) 
      {
          _tone(i, noteDuration, silentDuration);
      }
  }
}

void _sing(int singId)
{
    switch (singId) 
    {
      case 1: //K 1 
        sing(S_connection);
        break;
      case 2: //K 2 
        sing(S_disconnection);
        break;
      case 3: //K 3 
        sing(S_surprise);
        break;
      case 4: //K 4 
        sing(S_OhOoh);
        break;
      case 5: //K 5  
        sing(S_OhOoh2);
        break;
      case 6: //K 6 
        sing(S_cuddly);
        break;
      case 7: //K 7 
        sing(S_sleeping);
        break;
      case 8: //K 8 
        sing(S_happy);
        break;
      case 9: //K 9  
        sing(S_superHappy);
        break;
      case 10: //K 10
        sing(S_happy_short);
        break;  
      case 11: //K 11
        sing(S_sad);
        break;   
      case 12: //K 12
        sing(S_confused);
        break; 
      case 13: //K 13
        sing(S_fart1);
        break;
      case 14: //K 14
        sing(S_fart2);
        break;
      case 15: //K 15
        sing(S_fart3);
        break;    
      case 16: //K 16
        sing(S_mode1);
        break; 
      case 17: //K 17
        sing(S_mode2);
        break; 
      case 18: //K 18
        sing(S_mode3);
        break;   
      case 19: //K 19
        sing(S_buttonPushed);
        break;                      
      default:
        break;
    }
}

void sing(int songName)
{
  switch(songName)
  {
    case S_connection:
      _tone(note_E5,50,30);
      _tone(note_E6,55,25);
      _tone(note_A6,60,10);
    break;

    case S_disconnection:
      _tone(note_E5,50,30);
      _tone(note_A6,55,25);
      _tone(note_E6,50,10);
    break;

    case S_buttonPushed:
      bendTones (note_E6, note_G6, 1.03, 20, 2);
      delay(30);
      bendTones (note_E6, note_D7, 1.04, 10, 2);
    break;

    case S_mode1:
      bendTones (note_E6, note_A6, 1.02, 30, 10);  //1318.51 to 1760
    break;

    case S_mode2:
      bendTones (note_G6, note_D7, 1.03, 30, 10);  //1567.98 to 2349.32
    break;

    case S_mode3:
      _tone(note_E6,50,100); //D6
      _tone(note_G6,50,80);  //E6
      _tone(note_D7,300,0);  //G6
    break;

    case S_surprise:
      bendTones(800, 2150, 1.02, 10, 1);
      bendTones(2149, 800, 1.03, 7, 1);
    break;

    case S_OhOoh:
      bendTones(880, 2000, 1.04, 8, 3); //A5 = 880
      delay(200);

      for (int i=880; i<2000; i=i*1.04) {
           _tone(note_B5,5,10);
      }
    break;

    case S_OhOoh2:
      bendTones(1880, 3000, 1.03, 8, 3);
      delay(200);

      for (int i=1880; i<3000; i=i*1.03) {
          _tone(note_C6,10,10);
      }
    break;

    case S_cuddly:
      bendTones(700, 900, 1.03, 16, 4);
      bendTones(899, 650, 1.01, 18, 7);
    break;

    case S_sleeping:
      bendTones(100, 500, 1.04, 10, 10);
      delay(500);
      bendTones(400, 100, 1.04, 10, 1);
    break;

    case S_happy:
      bendTones(1500, 2500, 1.05, 20, 8);
      bendTones(2499, 1500, 1.05, 25, 8);
    break;

    case S_superHappy:
      bendTones(2000, 6000, 1.05, 8, 3);
      delay(50);
      bendTones(5999, 2000, 1.05, 13, 2);
    break;

    case S_happy_short:
      bendTones(1500, 2000, 1.05, 15, 8);
      delay(100);
      bendTones(1900, 2500, 1.05, 10, 8);
    break;

    case S_sad:
      bendTones(880, 669, 1.02, 20, 200);
    break;

    case S_confused:
      bendTones(1000, 1700, 1.03, 8, 2); 
      bendTones(1699, 500, 1.04, 8, 3);
      bendTones(1000, 1700, 1.05, 9, 10);
    break;

    case S_fart1:
      bendTones(1600, 3000, 1.02, 2, 15);
    break;

    case S_fart2:
      bendTones(2000, 6000, 1.02, 2, 20);
    break;

    case S_fart3:
      bendTones(1600, 4000, 1.02, 2, 20);
      bendTones(4000, 3000, 1.02, 2, 20);
    break;
  }
} 

