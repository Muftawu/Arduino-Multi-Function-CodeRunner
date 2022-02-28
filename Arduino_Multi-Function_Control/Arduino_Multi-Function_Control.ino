// Multi-function Arduino
#include "Servo.h"
#include "MFRC522.h"
#include "Stepper.h"
#include "AccelStepper.h"
#include "LiquidCrystal.h"
#include "IRremote.h"
#include "SPI.h"

#define RST_PIN 5
#define SS_PIN  53

#define STEPS 2048
#define receiver 40
#define motorSpeed 17

#define CLK  30
#define DT  31
#define SW  32

Servo servo;
IRrecv remote(receiver);
decode_results results;
MFRC522 card(SS_PIN, RST_PIN);
Stepper stepper(STEPS, 45, 46, 47, 48);

LiquidCrystal lcd(7,8,9,10,11,12);

int lastCLK, currentCLK, buttonState;
int counter = 0;
String counterDir = "";
unsigned long lastButtonPressed;

void setup () {
  Serial.begin(9600);
  Serial.setTimeout(10);
  // RFID CARD DETAILS
  while (Serial.available() ) {};  // do nothing if no serial port is opened
  SPI.begin();
  card.PCD_Init();
  card.PCD_DumpVersionToSerial();

  // STEPPER MOTOR DETAILS
  stepper.setSpeed(motorSpeed);

  // IR REMOTE DETAILS
  remote.enableIRIn();

  // ROTARY ENCODER DETAILS
  lastCLK = digitalRead(CLK);

  // LCD DETAILS 
  lcd.begin(16,1);
  lcd.setCursor(0,1);
  lcd.print("Scanning");
}

void loop () {
RFID_reader();
}

void RFID_reader() {
  //check for available cards
  if (!card.PICC_IsNewCardPresent()) {
    return;
  }

  // if available read card serial
  if (!card.PICC_ReadCardSerial()) {
    return;
  }

  // processing the received card UID for authentication
  Serial.print("Card UID:   ");
  String content;
  byte letter;
  for (byte a = 0; a < card.uid.size; a++) {
    Serial.print(card.uid.uidByte[a] < 0x10 ? " 0" : " ");
    Serial.print(card.uid.uidByte[a], HEX);
    content.concat(String(card.uid.uidByte[a] < 0x10 ? " 0" : " "));
    content.concat(String(card.uid.uidByte[a], HEX));
  }
  Serial.println();
  Serial.print("Message:   ");
  content.toUpperCase();
  if (content.substring(1) == "33 27 8B 1A") {
    Serial.println("Access Granted");
    lcd.setCursor(0,1);
    lcd.print("Access Granted");
    Serial.write("granted");
  }
  else {
    Serial.println("Access Denied");
    lcd.setCursor(0,1);
    lcd.print("Access Denied");
    Serial.write("denied");
  }
}

void Run_Stepper() {
  stepper.step(STEPS / 4);
  delay(500);
  stepper.step(-STEPS / 4);
  delay(500);
}

void IR_remote() {
  if (remote.decode(&results)) {
    // Serial.println(results.value, HEX);
    switch (results.value) {
      case 0xFF697D:
        Serial.println("You pressed 0");     // check HEX values later
        break;

      case 0xFF897A:
        Serial.println("You pressed 1");        // check HEX values later
        break;

    default:
        Serial.print("You pressed a key");
        break;
    }
  }
}

void Rotary_Encoder () {
  currentCLK = digitalRead(CLK);

  if (lastCLK != currentCLK && currentCLK == 1) {
    if (digitalRead(DT) != lastCLK) {
      counter --;
      counterDir = "CCW";
    }
    else {
      counter ++;
      counterDir = "CW";
    }
    Serial.print("Counter:  ");
    Serial.print(counter);
    Serial.print("   |  Direction:   ");
    Serial.println(counterDir);
  }
  lastCLK = currentCLK;
  buttonState = digitalRead(SW);
  if (buttonState == LOW) {
    if (millis() - lastButtonPressed > 50) {
      Serial.println("Button Pressed");
    }
    lastButtonPressed =  millis();
  }
  delay(1);
}

void Servo_Laser_Turret() {
  // pass 
}

int parseDataX_Servo(String data) {
  data.remove(data.indexOf("Y"));
  data.remove(data.indexOf("X"),1);
  return data.toInt();
}

int parseDataY_Servo(String data) {
  data.remove(data.indexOf("X"));
  return data.toInt();
}


/*LIST OF ALL WORKING FUNCTIONS
 1. RFID CARD READER 
 2. LCD INTEGRATION WITH RFID CARD READER 
 
 */
