//Libraries for fingerprint sensor
#include <Adafruit_Fingerprint.h>
#include <Softwareserial.h>
//defining fingerprint
SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

//Libraries for LCD display
#include <LiquidCrystal_I2C.h>
//defining the i2c lcd
LiquidCrystal_I2C lcd(0x3F, 16, 2);

//Libraries for Keypad
#include <Wire.h>
#include <Keypad_I2C.h>

//Keypad i2c address
#define I2CADDR 0x20
//defining the i2c keypad
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};
byte rowPins[ROWS] = { 0, 1, 2, 3 };
byte colPins[COLS] = { 4, 5, 6 };
Keypad_I2C kypd(makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CADDR, PCF8574);


//Library for door stepper motor
#include <AccelStepper.h>
// Define the door stepper motor and the pins it will use
AccelStepper stepper(AccelStepper::FULL4WIRE, 7, 6, 5, 4);

//defining pins for key chamber stepper motor(without a library)
int Bm = 8;
int Bp = 9;
int Ap = 10;
int Am = 11;

//defining variable for push buttons
int buttonValue;

//defining the array for the states of the key chamber whether the chamber is empty or not
int states[12] = {};

//defining pins for hall effect with
int data = A0;
int latch_hall = A2;
int clk_hall = A1;
int pulse = 10;

// For communication between Arduino and NodeMCU
#include <SoftwareSerial.h>
SoftwareSerial espard(12, 13);
String serial_data;

// For LED
String LedSend;

// For WiFi
String wifiSend;
String finger_id = "";  // Fingerprint ID variable
String key_num = "";    // Keypad input variable
int key_taken = "";     // Take or keep



void setup() {
  // put your setup code here, to run once
  Serial.begin(9600);

  //setting up the fingerprint
  finger.begin(57600);

  // Node connection
  espard.begin(9600);
  //Initializing LCD
  lcd.init();

  //setting up the keypad
  Wire.begin();
  kypd.begin(makeKeymap(keys));

  //setting up the door stepper motor
  stepper.setMaxSpeed(1100);
  stepper.setAcceleration(1000);

  //setting up the key chamber stepper
  pinMode(Bm, OUTPUT);  //b-
  pinMode(Bp, OUTPUT);  //b+
  pinMode(Ap, OUTPUT);  //a+
  pinMode(Am, OUTPUT);  //a-
  digitalWrite(Am, LOW);
  digitalWrite(Ap, LOW);
  digitalWrite(Bm, LOW);
  digitalWrite(Bp, LOW);

  //setting up the hall effect pins
  pinMode(latch_hall, OUTPUT);
  pinMode(clk_hall, OUTPUT);
  pinMode(data,INPUT);

  //initializing the key states to the states array
  readsr();
}

int ownerId = 20;
int key_hall;


void loop() {
  
//LCD display Push in or out
// lcd.backlight();
// // lcd show choose mode
// if(get_hall_no1()){
//   Serial.println("yay");
//   lcd_master_pwd();
//   if(get_hall_no() == 1234){
//     // registration code goes here
//     Serial.println("registration code");
//   }
// }
reg :
    Serial.println("yay");
    lcd_master_pwd();
    if(get_hall_no() == 1234){
    // registration code goes here
      Serial.println("registration code");
    }

push:
  push_lcd();
  //taking analog value of the 2 buttons
  buttonValue = analogRead(A3);

  //if the registered user is coming to take a key
  if (buttonValue >= 1020 && buttonValue <= 1025) {
    //LCD display enter your fingerprint:
    lcd.clear();

finger:
    enter_fingerprint_lcd();
    mySerial.listen();
    ownerId = getFingerprintID();
    if (ownerId == 20) {
      goto finger;
    } else if (ownerId == 0) {
      //LCD display try again
      tryagain_lcd();
      delay(2000);
      goto finger;
    } else if (ownerId <= 12 && ownerId >= 1) {
      finger_id = String(ownerId);
      //LCD display finger register successful
      finger_success_lcd();
      delay(3000);

//LCD display enter lecture hall number:
      key:
      enter_hall_lcd();
      delay(2000);

      //getting the key number from the user
      key_hall = get_hall_no();

      //checking the signal from the hall effect sensors
      readsr();

      //checking the hall number is valid or not
      if (key_hall < 0) {  // checking the return value of the function
        invalid_hall_lcd();
        delay(2000);
        goto key;
      } else if (key_hall == 0) {
        no_access_lcd();
        delay(2000);
        goto key1;
      }

      else if (states[key_hall] == 0) {
        hall_empty_lcd();
        goto key;
      }
      wait_lcd();
      //stepper motor rotates the exact key chamber
      chamber(key_hall);

       key_num = String(key_hall);
      //Locking the chamber by the servo
      espard.listen();
      //sendservo("up");

      //stepper motor opens the door
      doorDown();
      take_lcd();
      delay(3000);

      key_taken = 0;

      //hall effect sensor sends the signal when the key is taken
      // label:
      // readsr();
      // if(states[key_hall-1]!=0){
      //   goto label;
      // }
      // delay(2000);

      // Signal to switch on the red LED of the correct chamber
      sendled();
      Serial.println(LedSend);

      // Sending data to Node
      sendwifi();
      Serial.println(wifiSend);

      //stepper motor closes the door
      doorUp();

      // Unlocking the chamber by the servo
      //sendservo("down");

      thank_lcd();
      delay(4000);
      //chamber stepper motor rotates to the 1st chamber
      chamber(1);
      lcd.clear();
      goto push;

    } else {
      goto finger;
    }
    lcd.clear();
    goto push;
  }


  //if the registered user is coming to keep the key
  else if (buttonValue >= 28 && buttonValue <= 34) {
    //LCD display enter your fingerprint:
    lcd.clear();

finger1:

    enter_fingerprint_lcd();
    mySerial.listen();
    ownerId = getFingerprintID();

    if (ownerId == 20) {
      goto finger1;
    } else if (ownerId <= 12 && ownerId >= 1) {
      finger_id = String(ownerId);
      //LCD display finger register succecssful
      finger_success_lcd();
      delay(1500);

//LCD display enter lecture hall number:
key1:
      enter_hall_lcd();
      delay(1500);

      //getting the key number from the user
      key_hall = get_hall_no();

      //checking the signal from the hall effect sensors
      readsr();

      //checking the hall number is valid or not
      if (key_hall < 0) {  // checking the return value of the function
        invalid_hall_lcd();
        delay(2000);
        goto key1;
      } else if (key_hall == 0) {
        no_access_lcd();
        delay(2000);
        goto key1;
      }

      else if (states[key_hall] == 0) {
        not_empty_lcd();
        goto key1;
      } else {

        wait_lcd();
        //stepper motor rotates the exact key chamber
        chamber(key_hall);

        key_num = String(key_hall);
        // Locking the chamber by the servo
        espard.listen();
        //sendservo("up");

        //stepper motor opens the door
        doorDown();
        keepkey_lcd();

        key_taken = 1;

        //hall effect sensor sends the signal when the key is kept
        // label1:
        // readsr();
        // if(states[key_hall-1]!=1){
        //   goto label1;
        // }
        // delay(2000);

        // Signal to switch off the red LED of the correct chamber
        sendled();
        Serial.println(LedSend);

        // Sending data to Node
        sendwifi();
        Serial.println(wifiSend);

        //stepper motor closes the door
        doorUp();

        // Unlocking the chamber by the servo
        //sendservo("down");

        //LCD display thank you
        thank_lcd();
        delay(4000);

        //chamber stepper motor rotates to the 1st chamber
        chamber(1);
        lcd.clear();
        goto push;
      }

    } else {
      goto finger1;
    }
    lcd.clear();
    goto push;
  }
  Serial.println("loop end");

  if(get_hall_no1()){
    goto reg;
  }
}



//function definitions of the main loop

//LCD Functions
//display push in or out
void push_lcd() {
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("Push The");
  lcd.setCursor(0, 1);
  lcd.print("In or Out Button");
}
//enter the fingerprint
void enter_fingerprint_lcd() {
  lcd.setCursor(2, 0);
  lcd.print("Please Enter");
  lcd.setCursor(0, 1);
  lcd.print("The Fingerprint");
}
//try again
void tryagain_lcd() {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Try Again");
}
//fingerprint successful
void finger_success_lcd() {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Fingerprint");
  lcd.setCursor(3, 1);
  lcd.print("successful");
}
//enter lecture hall number
void enter_hall_lcd() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Enter Lecture");
  lcd.setCursor(2, 1);
  lcd.print("Hall Number");
}
//invalid hall number
void invalid_hall_lcd() {
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Invalid");
  lcd.setCursor(2, 1);
  lcd.print("Hall Number");
}
//hall key is already taken
void hall_empty_lcd() {
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("The Hall key is");
  lcd.setCursor(1, 1);
  lcd.print("already taken");
}
//take the key
void take_lcd() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Take The Key");
}
//thank you
void thank_lcd() {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Thank You!");
}
//key chamber occupied
void not_empty_lcd() {
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("The Hall is");
  lcd.setCursor(0, 1);
  lcd.print("Already Occupied");
}
void keepkey_lcd() {
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Keep the");
  lcd.setCursor(6, 1);
  lcd.print("key");
}
void wait_lcd() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Access Granted.");
  lcd.setCursor(1, 1);
  lcd.print("Wait a bit..");
}
void no_access_lcd() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Access Denied");
}
//Keypad function with lcd
int get_hall_no() {
  String inputString;
  int inputInt;
  int col = 1;
  lcd.clear();
  while (true) {
    char key = kypd.getKey();
    if (key) {
      if (key >= '0' && key <= '9') {
        inputString += key;
        lcd.setCursor(col++, 0);
        lcd.print(key);
      } else if (key == '#') {
        lcd.clear();
        if (inputString.length() > 0) {
          inputInt = inputString.toInt();
          inputString = "";  // clear input

          if (inputInt == ownerId) {
            return inputInt;
          } else if (inputInt >= 1 && inputInt <= 12) {
            return 0;
          }
          return -2;  //invalid hall
        }
        return -1;  //please enter hall number
      } else if (key == '*') {
        lcd.clear();
        inputString = "";
        col = 1;  // clear input
      }
    }
  }
}

int get_hall_no1() {
  String inputString;
  int inputInt;
  int col = 1;
  lcd.clear();
  while (true) {
    char key = kypd.getKey();
    if (key) {
      // if (key >= '0' && key <= '9') {
      //   inputString += key;
      //   lcd.setCursor(col++, 0);
      //   lcd.print(key);
      // } else if (key == '#') {
      //   lcd.clear();
      //   if (inputString.length() > 0) {
      //     inputInt = inputString.toInt();
      //     inputString = "";  // clear input

      //     if (inputInt == ownerId) {
      //       return inputInt;
      //     } else if (inputInt >= 1 && inputInt <= 12) {
      //       return 0;
      //     }
      //     return -2;  //invalid hall
      //   }
      //   return -1;  //please enter hall number
      // }
      inputString += key; 
       if (key == '*#') {
        // lcd.clear();
        // lcd should show "enter master pwd"
        // inputString = "";
        // col = 1;  // clear input
        return true;
      }
    }
  }
}
int get_hall_no2() {
  String inputString;
  int inputInt;
  int col = 1;
  lcd.clear();
  while (true) {
    char key = kypd.getKey();
    if (key) {
      if (key >= '0' && key <= '9') {
        inputString += key;
        lcd.setCursor(col++, 0);
        lcd.print(key);
      } else if (key == '#') {
        lcd.clear();
        if (inputString.length() > 0) {
          inputInt = inputString.toInt();
          inputString = "";  // clear input

          if (inputInt == ownerId) {
            return inputInt;
          } else if (inputInt >= 1 && inputInt <= 12) {
            return 0;
          }
          return -2;  //invalid hall
        }
        return -1;  //please enter hall number
      }
      // inputString += key; 
      // else if (key == '*#') {
      //   // lcd.clear();
      //   // lcd should show "enter master pwd"
      //   // inputString = "";
      //   // col = 1;  // clear input
      //   return true;
      // }
    }
  }
}
// lcd for pwd
void lcd_master_pwd(){
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Enter Master ");
  lcd.setCursor(4,0);
  lcd.print("Password");
}
//Door motor function
//door down function
void doorDown() {
  stepper.moveTo(16 * 1500);
  while (stepper.currentPosition() != 16 * 1500 - 100)
    stepper.run();
  stepper.stop();
  stepper.runToPosition();
}
//door up function
void doorUp() {
  stepper.moveTo(0);
  while (stepper.currentPosition() != +100)
    stepper.run();
  stepper.stop();
  stepper.runToPosition();
}

//hall effect functions
void clk_hallpulse() {
  digitalWrite(clk_hall, HIGH);
  delayMicroseconds(pulse);
  digitalWrite(clk_hall, LOW);
}
void readsr() {

  digitalWrite(latch_hall, LOW);
  digitalWrite(latch_hall, HIGH);


  digitalWrite(latch_hall, LOW);
  delayMicroseconds(50);
  digitalWrite(latch_hall, HIGH);


  for (int i = 0; i < 12; i++) {
    states[i] = !digitalRead(data);
    clk_hallpulse();
  }
}




//key chamber stepper motor functions
void chamber(int x) {
  if (x == 1) {
    if (key_hall - 1 >= 6) {
      for (int i = 0; i < 12 - x + 1; i++) {
        turnclk(16);
      }
    } else {
      for (int i = 0; i < x; i++) {
        turnanticlk(16);
      }
    }
  }
  if (x > 6) {
    for (int i = 0; i < 12 - x + 1; i++) {
      turnanticlk(16);
    }
  }
  if (x > 1 && x <= 6) {
    for (int i = 0; i < x - 1; i++) {
      turnclk(16);
    }
  }
}

void turnclk(int x) {
  for (int i = 0; i < x / 4; i++) {
    digitalWrite(Bm, LOW);
    digitalWrite(Ap, HIGH);
    delay(100);
    digitalWrite(Ap, LOW);
    digitalWrite(Bp, HIGH);
    delay(100);
    digitalWrite(Bp, LOW);
    digitalWrite(Am, HIGH);
    delay(100);
    digitalWrite(Am, LOW);
    digitalWrite(Bm, HIGH);
    delay(100);
  }
  digitalWrite(Am, HIGH);
  delay(2000);
  digitalWrite(Am, LOW);
  delay(1000);
}
void turnanticlk(int x) {
  for (int i = 0; i < x / 4; i++) {
    digitalWrite(Ap, LOW);
    digitalWrite(Bm, HIGH);
    delay(100);
    digitalWrite(Bm, LOW);
    digitalWrite(Am, HIGH);
    delay(100);
    digitalWrite(Am, LOW);
    digitalWrite(Bp, HIGH);
    delay(100);
    digitalWrite(Bp, LOW);
    digitalWrite(Ap, HIGH);
    delay(100);
  }
  digitalWrite(Bp, HIGH);
  delay(2000);
  digitalWrite(Bp, LOW);
  delay(1000);
}


//fingerprint functions
uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return 20;
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return 20;
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return 20;
      return p;
    default:
      Serial.println("Unknown error");
      return 20;
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return 20;
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return 20;
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return 20;
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return 20;
      return p;
    default:
      Serial.println("Unknown error");
      return 20;
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return 20;
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return 0;
    return p;
  } else {
    Serial.println("Unknown error");
    return 20;
    return p;
  }

  // found a match!
  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);
  int ownerId = finger.fingerID;

  //returns the fingerprint ID of the user
  return finger.fingerID;
}

// for communication betweeen arduino and nodemcu
void GetSerialData() {
  serial_data = "";
  while (serial_data == "") {
    while (espard.available() > 0) {
      serial_data = espard.readStringUntil('\n');
    }
  }
  serial_data.trim();
}

// for led
void PrepareLed() {
  LedSend = "";
  for (int i = 0; i < 12; i++) {
    if (i == 11) {
      LedSend += String(states[i]);
    } else {
      LedSend += String(states[i]) + ",";
    }
  }
}

void sendled() {
  PrepareLed();
  espard.println("led");
  GetSerialData();
  if (serial_data == "OK") {
    espard.println(LedSend);
  }
}

// for wifi
void PrepareWifi() {
  wifiSend = "";
  wifiSend += finger_id + ",";
  wifiSend += key_num + ",";
  wifiSend += key_taken;
}

void sendwifi() {
  PrepareWifi();
  espard.println("wifi");
  GetSerialData();
  if (serial_data == "OK") {
    espard.println(wifiSend);
  }
}

// for servo
void sendservo(String servo_state) {
  espard.println("servo");
  GetSerialData();
  if (serial_data == "OK") {
    espard.println(servo_state);
  }
}