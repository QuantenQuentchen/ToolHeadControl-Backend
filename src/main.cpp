#include <Arduino.h>
//#include <EEPROM.h> // EEPROM library

#define EEPROM_SIZE 1024 // EEPROM size in bytes
#define EEPROM_ADDRESS 0 // EEPROM address

/*
Magic Bytes for Serial Communication
Alles selbst geschrieben btw herr Weidner
*/
//#define OpeningByte 119
//#define ClosingByte "c"

#define OpeningByte 's'
#define ClosingByte 'q'

//#define fGetPosByte 1
#define fSetPosByte 1
#define fUpdatePosByte 2
#define fLockByte 3
#define fUnlockByte 4
#define fSetZeroByte 5
//#define updateStreamByte 7


#define YplaneSensorPin A0 // Y-plane sensor pin
#define YplaneSensorThreshold 10 // Y-plane sensor threshold
#define YplaneStepperPin 2 // Y-plane stepper pin
#define YplaneStepperDirPin 3 // Y-plane stepper direction pin
//Figure out how to stepper

#define XplaneSensorPin A1 // X-plane sensor pin
#define XplaneSensorThreshold 10 // X-plane sensor threshold
#define XplaneStepperPin 4 // X-plane stepper pin
#define XplaneStepperDirPin 5 // X-plane stepper direction pin
//Figure out how to stepper 2

int XplaneSensorValue; // X-plane sensor value
int YplaneSensorValue; // Y-plane sensor value
int YStepperRange = 100; // Y-plane stepper range in degrees
int XStepperRange = 100; // X-plane stepper range in degrees
int YStepPerDegree; // Y-plane stepper step per degree
int XStepPerDegree; // X-plane stepper step per degree
bool success;
bool readBytes;
bool settingCords;
bool XCalibrated;
bool YCalibrated;
bool mirror;

char BTbuffer;

byte Xhigh;
byte Xlow;
byte Yhigh;
byte Ylow;

unsigned long XComb;
unsigned long YComb;

int mode;
int readPointer;
//0 = Y-plane
//1 = X-plane
struct Cords {
  int y;
  int x;
};
bool lock;

String Xstr;
String Ystr;

struct command {
  int FuncType;
  int x;
  int y;
  bool lock;
  bool executed;
};

Cords cords;
Cords targetCords;
Cords zeroCords;
Cords diffCords;
command currentCommand;
//Async Timer Setup
int XAddTimer = 0;
bool XAddTimerActive = false;

int XSubTimer = 0;
bool XSubTimerActive = false;

int YAddTimer = 0;
bool YAddTimerActive = false;

int YSubTimer = 0;
bool YSubTimerActive = false;

int NullYTimer = 0;
bool NullYTimerActive = false;

int NullXTimer = 0;
bool NullXTimerActive = false;

void setup() {
  lock = false;
  //Setup Shit
  //Set Zero Cords

  //testByte = 0x49;
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  delay(1000);
  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(13, LOW);

  digitalWrite(12, HIGH);
  delay(1000);
  digitalWrite(12, LOW);

  digitalWrite(10, HIGH);
  delay(1000);
  digitalWrite(10, LOW);

  digitalWrite(9, HIGH);
  delay(1000);
  digitalWrite(9, LOW);

  pinMode(YplaneStepperPin, OUTPUT);
  pinMode(YplaneStepperDirPin, OUTPUT);
  pinMode(YplaneSensorPin, INPUT);

  pinMode(XplaneStepperPin, OUTPUT);
  pinMode(XplaneStepperDirPin, OUTPUT);
  pinMode(XplaneSensorPin, INPUT);
  
  zeroCords.x = 0;
  zeroCords.y = 0;
  //Set Target Cords
  targetCords.x = 0;
  targetCords.y = 0;
  //Set Current Cords
  cords.x = 0;
  cords.y = 0;
  //Set Diff Cords
  diffCords.x = 0;
  diffCords.y = 0;

  XCalibrated = false;
  YCalibrated = false;

  settingCords = false;
  readPointer = 0;
  readBytes = false;
  //setCords(zeroCords);
  Serial.begin(9600);  //serieller Monitor wird gestartet, Baudrate auf 9600 festgelegt
  //Serial.println("Es lebtt!");
  Xstr = "";
  Ystr = "";
  // put your setup code here, to run once:
}

void updateLoop(){
  /*
  Serial.print("DiffCordsX: ");
  Serial.println(diffCords.x);
  Serial.print("CordsX: ");
  Serial.println(cords.x);
  Serial.print("DiffCordsY: ");
  Serial.println(diffCords.y);
  Serial.print("CordsY: ");
  Serial.println(cords.y);
  */
  if(!settingCords){
  if(diffCords.x > 0) {
    //Serial.println(diffCords.x-1 >= cords.x);
    moveAddX();
    //diffCords.x--;
  } else if(diffCords.x < 0) {
    moveSubX();
    //diffCords.x++;
  } else if(diffCords.y > 0) {
    moveAddY();
    //diffCords.y--;
  } else if(diffCords.y < 0) {
    moveSubY();
    //diffCords.y++;
  } else {
    success = true;
  }
}
}

void setCordsLoop(){
  if (settingCords){
    if(cords.x <= targetCords.x-1) {
      moveAddX();
    } else if(cords.x >= targetCords.x+1) {
      moveSubX();
    } else if(cords.y <= targetCords.y-1) {
      moveAddY();
    } else if(cords.y >= targetCords.y+1) {
      moveSubY();
    } else {
      settingCords = false;
    }
  }
}

void readInput(){
  if(Serial.available()){ //wenn Daten empfangen werden...      
  BTbuffer = Serial.read(); //...sollen diese ausgelesen werden
  //Serial.println(BTbuffer);
  if(BTbuffer == OpeningByte) {
    readBytes = true;
    readPointer = 0;
    Xstr = "";
    Ystr = "";
  }
  if(BTbuffer == ClosingByte) {
    readPointer = 0;
    readBytes = false;
    currentCommand.executed = false;
    mirror = true;
  }
  if(readBytes) {
    if(readPointer == 1){
      currentCommand.FuncType = BTbuffer-'0';
    }
    if(readPointer >= 2 && readPointer <= 6){
      Xstr += BTbuffer;
    }
    if(readPointer >= 7 && readPointer <= 11){
      Ystr += BTbuffer;
    }
    if(readPointer == 12){
      currentCommand.lock = BTbuffer;
      currentCommand.x = Xstr.toInt();
      currentCommand.y = Ystr.toInt();
      }
    readPointer++;
  }
}

}

void mirrorCords(){
  if(true){
    Serial.print("s");
    Serial.print(cords.x);
    Serial.print(" ");
    Serial.print(cords.y);
    Serial.print(" ");
    Serial.print(lock);
    Serial.print("q");
    mirror = false;
  }
}

void loop(){
  readInput();
  mirrorCords();
  commandProcessor();

  updateLoop();
  setCordsLoop();
}

void commandProcessor(){
  if(!currentCommand.executed){
    if(currentCommand.FuncType == fSetPosByte){
      targetCords.x = currentCommand.x;
      targetCords.y = currentCommand.y;
      settingCords = true;
    }
    if(currentCommand.FuncType == fUpdatePosByte){
      diffCords.x += currentCommand.x;
      diffCords.y += currentCommand.y;
      //moving = true;
    }
    if(currentCommand.FuncType == fLockByte){
      lock = currentCommand.lock;
    }
    if(currentCommand.FuncType == fUnlockByte){
      lock = currentCommand.lock;
    }
    if(currentCommand.FuncType == fSetZeroByte){
      setNull();
    }
    currentCommand.executed = true;
  }
}


void moveAddY() {
  if(digitalRead(YplaneSensorPin) == HIGH) {
    //Serial.println("Hot");
    return;
  }
  if(YAddTimerActive == false){
    YAddTimer = millis();
    YAddTimerActive = true;
    digitalWrite(YplaneStepperDirPin, HIGH);
    digitalWrite(YplaneStepperPin, HIGH);
    digitalWrite(13,HIGH);
  }
  if(millis() - YAddTimer > 500){
  //delayMicroseconds(500);
  digitalWrite(YplaneStepperPin, LOW);
  digitalWrite(13,LOW);
  cords.y++;
  if(!settingCords){
    diffCords.y--;
    }
  YAddTimerActive = false;
  }
  //Move Y-plane stepper
  //digitalWrite(YplaneStepperDirPin, HIGH);
  //igitalWrite(YplaneStepperPin, HIGH);
  //delayMicroseconds(500);
  //digitalWrite(YplaneStepperPin, LOW);
  //cords.y++;
  //Serial.print("Moved Y+");
  //digitalWrite(13,HIGH);
  //delay(100);
  //digitalWrite(13,LOW);
  //Serial.println(cords.y);
  //Move step to pos+Y
}

void moveSubY() {
 if(cords.y <= -1 || digitalRead(YplaneSensorPin) == HIGH) {
  //Serial.println("Hot");
    return;
  }
  if(YSubTimerActive == false){
    YSubTimer = millis();
    YSubTimerActive = true;
    digitalWrite(YplaneStepperDirPin, LOW);
    digitalWrite(YplaneStepperPin, HIGH);
    digitalWrite(12,HIGH);
  }
  if(millis() - YSubTimer > 500){
  //delayMicroseconds(500);
  digitalWrite(YplaneStepperPin, LOW);
  digitalWrite(12,LOW);
  cords.y--;
   if(!settingCords){
    diffCords.y++;
    }
  YSubTimerActive = false;
  }
  //Serial.print("Moved Y-");

  delay(100);
  //Serial.println(cords.y);
}

void moveAddX(){
 if(digitalRead(XplaneSensorPin) == HIGH) {
  //Serial.println("Hot");
    return;
  }
  if(XAddTimerActive == false){
    XAddTimer = millis();
    XAddTimerActive = true;
    digitalWrite(XplaneStepperDirPin, HIGH);
    digitalWrite(XplaneStepperPin, HIGH);
    digitalWrite(9,HIGH);
  }
  if(millis() - XAddTimer > 500){
    digitalWrite(XplaneStepperPin, LOW);
    digitalWrite(9,LOW);
    cords.x++;
    XAddTimerActive = false;
  }
   if(!settingCords){
    diffCords.x--;
    }
  //digitalWrite(XplaneStepperDirPin, HIGH);
  //digitalWrite(XplaneStepperPin, HIGH);
  //delayMicroseconds(500);
  //digitalWrite(XplaneStepperPin, LOW);
  //cords.x++;
  //digitalWrite(9,HIGH);
  //delay(100);
  //digitalWrite(9,LOW);
  //Serial.print("Moved X+");
  //Serial.println(cords.x);
}

void moveSubX() {
 if(cords.x <= -1 || digitalRead(XplaneSensorPin) == HIGH) {
  //Serial.println("Hot");
    return;
  }

  if(XSubTimerActive == false){
    XSubTimer = millis();
    XSubTimerActive = true;
    digitalWrite(XplaneStepperDirPin, LOW);
    digitalWrite(XplaneStepperPin, HIGH);
    digitalWrite(10,HIGH);
  }
  if(millis() - XSubTimer > 500){
    digitalWrite(XplaneStepperPin, LOW);
    digitalWrite(10,LOW);
      if(!settingCords){
    diffCords.x++;
    }
    cords.x--;
    XAddTimerActive = false;
  }

  //digitalWrite(XplaneStepperDirPin, LOW);
  //digitalWrite(XplaneStepperPin, HIGH);
  //delayMicroseconds(500);
  //digitalWrite(XplaneStepperPin, LOW);
  //cords.x--;
  //Serial.print("Moved X-");
  //digitalWrite(10,HIGH);
  //Serial.println(cords.x);
}

void setNull() {
  //Set cords to zero
  while(true) {
    if(digitalRead(XplaneSensorPin)== LOW){
      //digitalWrite(XplaneStepperDirPin, LOW);
      //digitalWrite(XplaneStepperPin, HIGH);
      //delayMicroseconds(1000);
      //digitalWrite(XplaneStepperPin, LOW);
      if(NullXTimerActive == false){
        NullXTimer = millis();
        NullXTimerActive = true;
        digitalWrite(XplaneStepperDirPin, LOW);
        digitalWrite(XplaneStepperPin, HIGH);
        digitalWrite(13,HIGH);
      }
      if(millis() - NullXTimer > 500){
        digitalWrite(XplaneStepperPin, LOW);
        digitalWrite(13,LOW);
        //cords.x--;
        NullXTimerActive = false;
        }
    }
    else{
      //moveAddX();
      XCalibrated = true;
      cords.x = 0;
    }
    if(digitalRead(YplaneSensorPin)== LOW){
      //digitalWrite(YplaneStepperDirPin, LOW);

      if(NullYTimerActive == false){
        NullYTimer = millis();
        NullYTimerActive = true;
        digitalWrite(YplaneStepperDirPin, LOW);
        digitalWrite(YplaneStepperPin, HIGH);
        digitalWrite(10,HIGH);
      }
      if(millis() - NullYTimer > 500){
        digitalWrite(YplaneStepperPin, LOW);
        digitalWrite(10,LOW);
        //cords.x--;
        NullYTimerActive = false;
        }

      //digitalWrite(YplaneStepperPin, HIGH);
      //delayMicroseconds(1000);
      //digitalWrite(YplaneStepperPin, LOW);
    }
    else{
      //moveAddY();
      YCalibrated = true;
      cords.y = 0;
    }
    if(XCalibrated && YCalibrated){
      break;
    }
  }
}

bool setCords(Cords targetCords) {
  while(cords.y != targetCords.y) {
    if (cords.y > targetCords.y) {
      moveSubY();
    } else {
      moveAddY();
    }
  }
  while (cords.y != targetCords.y) {
    if (cords.y > targetCords.y) {
      moveSubY();
    } else {
      moveAddY();
    }
  }
  if(cords.x == targetCords.x && cords.y == targetCords.y) {
    return true;
  }
  //Set cords
}