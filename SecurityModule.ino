#include <AccelStepper.h>
#include <Stepper.h>
//#include <EasyTransfer.h>
#include <SoftEasyTransfer.h>
#include <SoftwareSerial.h>
#define STEPPER_PIN_1 9
#define STEPPER_PIN_2 10
#define STEPPER_PIN_3 11
#define STEPPER_PIN_4 12
#define VIBRATION_PIN A0
#define LASER_PIN 13
#define LASER_REC 13
//int stepsPerRev = 64;
//float degreePerRev = 5.625;
const int stepsPerRevolution = 2048;
const int rolePerMinute = 15;
int step_number = 0;
int val = 0;
int l = 0;
boolean isMoving = false; //boolean that keeps track of whether or not the lock is moving
boolean locked = true; //boolean for keeping track of the state of the lock
Stepper stepper = Stepper(stepsPerRevolution, STEPPER_PIN_1,STEPPER_PIN_3,STEPPER_PIN_2,STEPPER_PIN_4); //stepper object
struct sendPackage{ //struct to be sent over SoftEasyTransfer
  boolean laserBool;
  boolean lockBool;
  boolean vibrationBool;
  int vibration;
};
struct recievePackage{ //struct to receive over SoftEasyTransfer
  boolean unlockBool;
};
sendPackage sData;
recievePackage rData;
SoftEasyTransfer ETin, ETout; //Out and in objects for bi-directional communication
void openLock(){
  if(locked){
    Serial.println("opening lock");
    stepper.step(-(stepsPerRevolution/4)); //negative steps is counterclockwise
    Serial.println("lock is open");
    sData.lockBool = false;

  }
  
}
void closeLock(){
  if(!locked){
    Serial.println("closing lock");
    stepper.step((stepsPerRevolution/4)); //positive steps is clockwise
    Serial.println("lock is closed");
    sData.lockBool = true;
//    locked = true;
  }
  
}

SoftwareSerial BTSerial(6,7); // RX | TX
void setup() {
  stepper.setSpeed(rolePerMinute); //set speed
  ETout.begin(details(sData), &BTSerial); //begin software serial connections
  ETin.begin(details(rData),&BTSerial);
  pinMode(LASER_PIN,OUTPUT); //initialize pins
  pinMode(LASER_REC,INPUT);
  Serial.begin(9600);
  BTSerial.begin(9600);
  digitalWrite(LASER_PIN,HIGH);
//  closeLock();
//  openLock();
}

void loop() {
//Serial.println("loop");
if(ETin.receiveData()){
  if(rData.unlockBool != locked){
//    locked = !locked;
      locked = rData.unlockBool;
    Serial.println("state has changed");
    if(rData.unlockBool){
      Serial.println("unlocking now");
      openLock();
    } else if(!rData.unlockBool){
        Serial.println("locking this time");
        closeLock();
      }
  }
}
sData.vibration = analogRead(VIBRATION_PIN); //read vibration pin

sData.laserBool = digitalRead(LASER_REC); //read laser pin
Serial.println(sData.laserBool);

ETout.sendData();



}//endloop()
