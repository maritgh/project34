#include <OnewireKeypad.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9   
#define SS_PIN          10 
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;
byte blockAddr4 = 4;
byte blockAddr5 = 5;
byte buffer1[18];
byte len = 18;
char iban[36];

 
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
 
//original key bind
// char KEYS[] = {
//   '1', '2', '3', 'D',
//   '4', '5', '6', 'C',
//   '7', '8', '9', 'E',
//   'x', '0', 'x', 'x'
// };
 
/*Whatever key is pressed first is displayed*/
 
//Use this to get correct key output
char KEYS[] = {
  'D', '3', '2', '1',
  'C', '6', '5', '4',
  'E', '9', '8', '7',
  'x', 'X', '0', 'x'
};
 
OnewireKeypad <Print, 16> KP2(Serial, KEYS, 4, 4, A1, 4700, 1000);
 
void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
  KP2.SetKeypadVoltage(5.0);
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  // Prepare the key (used both as key A and as key B)
  // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
       
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

}

void cardscan(){
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()){
      for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
      }
      return;
    }
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr4, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("Authentication failed: ")); Serial.println(mfrc522.GetStatusCodeName(status));
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
    for (uint8_t i = 0; i < 16; i++)
    {
      if (buffer1[i] != 32)
      {
        iban[i]=buffer1[i];
      }
    }
    Serial.print(" ");
    memset(buffer1, 0, sizeof(buffer1));
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr5, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("Authentication failed: ")); Serial.println(mfrc522.GetStatusCodeName(status));
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
    for (uint8_t i = 0; i < 16; i++)
    {
      if (buffer1[i] != 32)
      {
        iban[i+8]=buffer1[i];
      }
    }
    memset(buffer1, 0, sizeof(buffer1));
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    Serial.println();
    Serial.println(iban);
    mfrc522.PCD_Reset();
    mfrc522.PCD_Init();
    delay(100);

}


void keypadpress(){
  Serial.print("keypad : ");
  char Key;
  if (Key = KP2.Getkey()) {
      Serial.println(Key);
      lcd.setCursor(0, 0);
      lcd.clear();
      lcd.print(Key);
    }
}


void buttonpress(int value_key){
  Serial.print("button : ");
  if (value_key > 850) {
    Serial.println("6");
  } else if (value_key > 800) {
    Serial.println("5");
  } else if (value_key > 700) {
    Serial.println("4");
  } else if (value_key > 600) {
    Serial.println("3");
  } else if (value_key > 450) {
    Serial.println("2");
  } else if (value_key < 450) {
    Serial.println("1");
  } else{
    Serial.println("Error");
  }
   delay(500);
}