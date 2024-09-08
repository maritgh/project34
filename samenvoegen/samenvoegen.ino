#include <Servo.h>
#include <Arduino.h>
#include <OnewireKeypad.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_Thermal.h>
#include <SoftwareSerial.h>
#include <DS3231.h>
//Motoren
// Motor A connections (€50 dispenser)
int enA = 12;  // Enable pin for motor A
int in1 = 22;  // Control pin 1 for motor A
int in2 = 23;  // Control pin 2 for motor A

// Motor B connections (€20 dispenser)
int enB = 13;  // Enable pin for motor B
int in3 = 24;  // Control pin 1 for motor B
int in4 = 25;  // Control pin 2 for motor B

// Servo connections
int servoPin50 = 10;  // Servo pin for the €50 dispenser
int servoPin20 = 9;   // Servo pin for the €20 dispenser
int servoPinDeur = 8;

const int IRSensorPin = 3;  // IR sensor pin to check if cash is dispensed

// Variables for dispenser counts and bank budget
int dispenser50Count = 13;
int dispenser20Count = 20;
int bankBudget = 1040;
bool transactionInProgress = false;  // Flag to indicate if a transaction is in progress

Servo Servo50;
Servo Servo20;
Servo Deur;
//rfid scanner
#define RST_PIN 5
#define SS_PIN 53
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.

MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;
byte blockAddr4 = 4;
byte blockAddr5 = 5;
byte buffer1[18];
byte len = 18;
char iban[36];
char uid[12];
String bon;

//bon printer
#define TX_PIN 19
#define RX_PIN 18

SoftwareSerial mySerial(RX_PIN, TX_PIN);
Adafruit_Thermal printer(&mySerial);

//keypad
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

//Use this to get correct key output
char KEYS[] = {
  'D', '3', '2', '1',
  'C', '6', '5', '4',
  'E', '9', '8', '7',
  '0', '0', '0', '0'
};

OnewireKeypad<Print, 16> KP2(Serial, KEYS, 4, 4, A1, 4700, 1000);

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
  KP2.SetKeypadVoltage(5.0);
  SPI.begin();         // Init SPI bus
  mfrc522.PCD_Init();  // Init MFRC522 card
  // Prepare the key (used both as key A and as key B)
  // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  mySerial.begin(9600);
  printer.begin();
  printer.setHeatConfig(11, 200, 90);
  printer.sleep();
  // Initialize the servos
  Servo50.attach(servoPin50);
  Servo50.write(0);  // Initialize the €50 dispenser servo to the start position

  Servo20.attach(servoPin20);
  Servo20.write(0);  // Initialize the €20 dispenser servo to the start position
  Deur.attach(servoPinDeur);
  Deur.write(0);
  // Initialization code for the motor drivers
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

  analogWrite(enA, 255);
  analogWrite(enB, 255);

  // Initialize the IR sensor
  pinMode(IRSensorPin, INPUT);
}

void loop() {

  cardscan();


  byte KState = KP2.Key_State();
  if (KState == PRESSED) {
    keypadpress();
  }

  int value_key = analogRead(A2);

  /*If two buttons are pressed at the time then lowest value is used*/

  if (value_key <= 900) {
    buttonpress(value_key);
  }
  if (Serial.read() == 'q') {
    bon = Serial.readStringUntil('q');
    Serial.println(bon);
  }
  if (!transactionInProgress && Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();                                  // Remove leading and trailing whitespace, including the newline character
    Serial.println("Received command: " + command);  // Debug message
    executeCommand(command);                         // Process the command
  }
}

void cardscan() {
  mfrc522.PCD_Init();
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    for (byte i = 0; i < 6; i++) {
      key.keyByte[i] = 0xFF;
    }
    return;
  }

  printHex(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.println();

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr4, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    for (byte i = 0; i < 6; i++) {
      key.keyByte[i] = 0xFF;
    }
    return;
  }
  status = mfrc522.MIFARE_Read(blockAddr4, buffer1, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    for (byte i = 0; i < 6; i++) {
      key.keyByte[i] = 0xFF;
    }
    return;
  }
  for (uint8_t i = 0; i < 16; i++) {
    if (buffer1[i] != 32) {
      iban[i] = buffer1[i];
    }
  }
  Serial.print(" ");
  memset(buffer1, 0, sizeof(buffer1));
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr5, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    for (byte i = 0; i < 6; i++) {
      key.keyByte[i] = 0xFF;
    }
    return;
  }
  status = mfrc522.MIFARE_Read(blockAddr5, buffer1, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    for (byte i = 0; i < 6; i++) {
      key.keyByte[i] = 0xFF;
    }
    return;
  }
  for (uint8_t i = 0; i < 16; i++) {
    if (buffer1[i] != 32) {
      iban[i + 8] = buffer1[i];
    }
  }
  memset(buffer1, 0, sizeof(buffer1));
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  Serial.println(iban);
  Serial.println(uid);
  mfrc522.PCD_Reset();
  delay(100);
}

void keypadpress() {
  // Serial.print("keypad : ");
  char Key;
  if (Key = KP2.Getkey()) {
    Serial.println(Key);
    lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print(Key);
  }
}

void buttonpress(int value_key) {
  // Serial.print("button : ");
  if (value_key > 850) {
    Serial.println("z");
  } else if (value_key > 800) {
    Serial.println("y");
  } else if (value_key > 700) {
    Serial.println("x");
  } else if (value_key > 600) {
    Serial.println("w");
  } else if (value_key > 450) {
    Serial.println("v");
  } else if (value_key < 450) {
    Serial.println("u");
  } else {
    Serial.println("Error");
  }
  delay(500);
}

void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
void executeCommand(String command) {
  // Split the command into tokens based on space
  while (command.length() > 0) {
    int spaceIndex = command.indexOf(' ');
    String denomination;
    if (spaceIndex != -1) {
      denomination = command.substring(0, spaceIndex);
      command = command.substring(spaceIndex + 1);
    } else {
      denomination = command;
      command = "";
    }
    int countIndex = command.indexOf(' ');
    String countString;
    if (countIndex != -1) {
      countString = command.substring(0, countIndex);
      command = command.substring(countIndex + 1);
    } else {
      countString = command;
      command = "";
    }
    int count = countString.toInt();
    if (denomination == "50") {
      withdraw50(count);  // Withdraw €50 notes
    } else if (denomination == "20") {
      withdraw20(count);  // Withdraw €20 notes
    } else if (denomination == "30") {
      printbon();
      return;
    } else {
      Serial.println("Invalid denomination.");
      return;
    }
  }
  Deur.write(180);
  Serial.println("geld uitgegeven");
  delay(10000);
  Deur.write(0);
}

void printbon() {
  printer.wake();

  delay(1000);  // Give the printer some time to initialize
  printer.println("\n");
  printer.justify('L');
  printer.setLineHeight(30);

  // Print the header
  printer.doubleHeightOn();
  printer.println(F("RTB"));
  printer.doubleHeightOff();
  delay(500);  // Increased delay

  // Print date and time
  printer.justify('C');
  printer.println(F("Date"));
  delay(200);  // Increased delay
  printer.justify('C');
  printer.println(F("Time"));
  delay(200);  // Increased delay

  // Print pasnummer
  printer.justify('C');
  printer.println(F("Card"));
  printer.underlineOn();
  delay(200);
  printer.justify('C');
  printer.println(F("Number:"));
  printer.underlineOn();
  delay(200);
  printer.println(F("NL XXXX"));
  printer.println(F("XXXX XX XX"));
  printer.underlineOff();
  delay(200);  // Increased delay

  // Print gepind bedrag with smaller font size
  printer.setFont(1);  // Set smaller font size
  printer.justify('C');
  printer.println(F("Pinned"));
  printer.underlineOn();
  printer.setFont(1);  // Set smaller font size
  printer.justify('C');
  printer.println(F("Amount"));
  printer.underlineOn();
  delay(200);

  printer.println(F("100,00"));
  printer.underlineOff();
  printer.setFont(0);  // Revert to the original font size
  delay(200);          // Increased delay

  printer.justify('C');
  printer.println(F("Thank\nyou"));
  printer.setLineHeight(50);

  printer.println(F("Raccoon"));

  printer.justify('C');
  printer.println(F("Trust &\nBank"));
  printer.println("\n");
  printer.println("\n");
  printer.sleep();
}
// Function to withdraw €50 notes
void withdraw50(int count) {
  Serial.println("Withdraw €50 notes: " + String(count));
  for (int i = 0; i < count; i++) {
    bool dispensed = false;
    Servo50.write(0);
    delay(500);
    // Turn on motor A
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    while (!dispensed) {
      // Move the servo to 180 degrees to dispense the note
      Servo50.write(180);

      // Set the transaction in progress
      transactionInProgress = true;

      // Wait for the dispensing to complete
      for (int i = 0; i < 50; i++) {
        delay(50);  // Adjust this delay as necessary
        if (digitalRead(IRSensorPin) == LOW) {
          dispensed = true;
        }
      }

      // Check if cash is dispensed
      if (digitalRead(IRSensorPin) == HIGH && dispensed == false) {  // HIGH means no cash
        Serial.println("Cash not detected, retrying...");


        Servo50.write(0);  // Move the servo back to 0 degrees
        for (int i = 0; i < 10; i++) {
          delay(50);  // Adjust this delay as necessary
          if (digitalRead(IRSensorPin) == LOW) {
            dispensed = true;
          }
        }
        Servo50.write(180);
      } else {
        Servo50.write(0);  // Move the servo back to 0 degrees
        dispensed = true;
      }

      // Add a delay between dispensing each note, adjust as necessary
      for (int i = 0; i < 10; i++) {
        delay(50);  // Adjust this delay as necessary
        if (digitalRead(IRSensorPin) == LOW) {
          dispensed = true;
        }
      }
    }
    // turn off motor A
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    Serial.println("briefje van 50");
    delay(1000);
  }

  // Set the transaction as complete for €50 notes
  transactionInProgress = false;
}

// Function to withdraw €20 notes
void withdraw20(int count) {
  Serial.println("Withdraw €20 notes: " + String(count));
  for (int i = 0; i < count; i++) {
    bool dispensed = false;
    Servo20.write(0);
    delay(500);
    // Turn on motor B
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    while (!dispensed) {
      // Move the servo to 180 degrees to dispense the note
      Servo20.write(180);

      // Set the transaction in progress
      transactionInProgress = true;

      // Wait for the dispensing to complete
      for (int i = 0; i < 50; i++) {
        delay(50);  // Adjust this delay as necessary
        if (digitalRead(IRSensorPin) == LOW) {
          dispensed = true;
        }
      }

      // Check if cash is dispensed
      if (digitalRead(IRSensorPin) == HIGH && dispensed == false) {  // HIGH means no cash
        Serial.println("Cash not detected, retrying...");

        Servo20.write(0);  // Move the servo back to 0 degrees
        for (int i = 0; i < 10; i++) {
          delay(50);  // Adjust this delay as necessary
          if (digitalRead(IRSensorPin) == LOW) {
            dispensed = true;
          }
        }
        Servo20.write(180);
      } else {


        Servo20.write(0);  // Move the servo back to 0 degrees
        dispensed = true;
      }

      // Add a delay between dispensing each note, adjust as necessary
      for (int i = 0; i < 10; i++) {
        delay(50);  // Adjust this delay as necessary
        if (digitalRead(IRSensorPin) == LOW) {
          dispensed = true;
        }
      }
    }
    // Turn off motor B
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
    Serial.println("briefje van 20");
    delay(1000);
  }
  // Set the transaction as complete for €20 notes
  transactionInProgress = false;
}