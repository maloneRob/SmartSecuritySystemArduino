#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <SoftEasyTransfer.h>

// Setup SoftwareSerial for bluetooth pins:
SoftwareSerial BTserial(2, 3); // RX | TX

// Initialize pins for the buzzer and button:
const int buzzerPin = 5;
const int buttonPin = 4;

// Create variables for button debouncing:
int buttonState;
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

// Create variables to hold the latest status of sensors:
boolean doorStatus = false;
boolean laserStatus = true;
boolean vibrationStatus = true;

// Initialize and setup LiquidCrystal library:
const int rs = 12, en = 11, d4 = 10, d5 = 9, d6 = 8, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Initialize the struct we will be recieving data with:
struct toRecieve {
  boolean laserBool;
  boolean lockBool;
  boolean vibrationBool;
  int vibration;
};

// Initialize the struct we will be sending data with:
struct toSend {
  boolean unlockBool;
};

// Declare SoftEasyTransfer input/output:
SoftEasyTransfer Etout, Etin;

// Initialize instances of the structs that will send and recieve data:
toSend dataSent;
toRecieve dataRecieved;

void setup() {
  pinMode(buzzerPin, INPUT);
  pinMode(buttonPin, INPUT);
  
  dataSent.unlockBool = false;
  dataRecieved.laserBool = true;
  dataRecieved.vibrationBool = true;
  
  Etout.begin(details(dataSent), &BTserial);
  Etin.begin(details(dataRecieved), &BTserial);
  
  lcd.begin(16,2);
  lcd.print("Welcome to smart");
  lcd.setCursor(0,1);
  lcd.print("Security!");
  
// start th serial communication with the host computer
 Serial.begin(9600);
// Serial.println("Arduino with HC-05 is ready");
  
  // start communication with the HC-05 using 9600
  BTserial.begin(9600);
  Serial.println("BTserial started at 9600");
  delay(5000);
  lcd.clear();
  lcd.print("Door: L ");
  lcd.print("laser: S ");
  lcd.setCursor(0,1);
  lcd.print("Vibration: S");
}

void loop() {
  if (Etin.receiveData()) {
    
  }
  //Serial.println(dataRecieved.vibration);
  
  if (dataSent.unlockBool != doorStatus || dataRecieved.laserBool != laserStatus || dataRecieved.vibrationBool != vibrationStatus) {
    lcd.clear();
    if (dataSent.unlockBool == true) {
      lcd.print("Door: U ");
    } else {
      lcd.print("Door: L ");
    }
    if (dataRecieved.laserBool == true) {
      noTone(buzzerPin);
      lcd.print("laser: S ");
    } else {
      tone(buzzerPin, 200);
      lcd.print("laser: T ");
    }
    lcd.setCursor(0,1);
    if (dataRecieved.vibration <= 800) {
      lcd.print("Vibration: S");
    } else {
      lcd.print("Vibration: T");
    }
  }
  doorStatus = dataSent.unlockBool;
  laserStatus = dataRecieved.laserBool;
  vibrationStatus = dataRecieved.vibrationBool;

  
  int reading = digitalRead(buttonPin);
  
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    
    if (reading != buttonState) {
      buttonState = reading;
      
      if (buttonState == HIGH) {
        // when unlock button pressed;
        dataSent.unlockBool = !dataSent.unlockBool;
        Serial.println(dataSent.unlockBool);
        Serial.println("PRESSED");
      }
    }
  }
  lastButtonState = reading;

  Etout.sendData();
}
