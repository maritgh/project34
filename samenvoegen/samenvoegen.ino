#include <OnewireKeypad.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h>
// #include "Adafruit_Thermal.h"
// #include "SoftwareSerial.h"
// #include <DS3231.h>
//rfid scanner
#define RST_PIN 9
#define SS_PIN 10
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
#define TX_PIN 6
#define RX_PIN 5

// SoftwareSerial mySerial(RX_PIN, TX_PIN);
// Adafruit_Thermal printer(&mySerial);

//keypad
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

//Use this to get correct key output
char KEYS[] = {
  'D', '3', '2', '1',
  'C', '6', '5', '4',
  'E', '9', '8', '7',
  'x', 'X', '0', 'x'
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
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
  printer.begin();
  printer.setHeatConfig(11, 200, 90);
  printer.sleep();
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