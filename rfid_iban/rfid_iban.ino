#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9   
#define SS_PIN          10  
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;
//    byte sector         = 1;

  MFRC522::StatusCode status;

void setup() {
    Serial.begin(9600);
    SPI.begin();
    mfrc522.PCD_Init();
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }
    Serial.println(F("Scan a MIFARE Classic PICC."));
}

void loop() {
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
        return;

    byte blockAddr4 = 4;
    byte blockAddr5 = 5;
    //byte dataBlock4[] = {0x4E, 0x4C, 0x30, 0x30, 0x42, 0x41, 0x4E, 0x4B, 0x00, 0X00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
   // byte dataBlock5[] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x32, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    //byte buffer[18];
    byte buffer1[16];
    //byte size = sizeof(buffer);
    byte len = 18;
    MFRC522::StatusCode status;
    // Authenticate and write to block 4
    // status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr4, &key, &(mfrc522.uid));
    // if (status != MFRC522::STATUS_OK) {
    //     Serial.print(F("Authentication failed: ")); Serial.println(mfrc522.GetStatusCodeName(status));
    //     return;
    // }
    // status = mfrc522.MIFARE_Write(blockAddr4, dataBlock4, 16);
    // if (status != MFRC522::STATUS_OK) {
    //     Serial.print(F("Write failed: ")); Serial.println(mfrc522.GetStatusCodeName(status));
    //     return;
    // }
    // Serial.println(F("Block 4 written"));

    // // Authenticate and write to block 5
    // status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr5, &key, &(mfrc522.uid));
    // if (status != MFRC522::STATUS_OK) {
    //     Serial.print(F("Authentication failed: ")); Serial.println(mfrc522.GetStatusCodeName(status));
    //     return;
    // }
    // status = mfrc522.MIFARE_Write(blockAddr5, dataBlock5, 16);
    // if (status != MFRC522::STATUS_OK) {
    //     Serial.print(F("Write failed: ")); Serial.println(mfrc522.GetStatusCodeName(status));
    //     return;
    // }
    // Serial.println(F("Block 5 written"));
    // mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, 1);
    // status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr4, buffer, &size);
    // if (status != MFRC522::STATUS_OK) {
    //     Serial.print(F("MIFARE_Read() failed: "));
    //     Serial.println(mfrc522.GetStatusCodeName(status));
    // }

    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr4, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("Authentication failed: ")); Serial.println(mfrc522.GetStatusCodeName(status));
        
        return;
    }
    status = mfrc522.MIFARE_Read(blockAddr4, buffer1, &len);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("Reading failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }
    for (uint8_t i = 0; i < 16; i++)
    {
      if (buffer1[i] != 32)
      {
        Serial.write(buffer1[i]);
      }
    }
    Serial.print(" ");
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr5, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
          Serial.print(F("Authentication failed: ")); Serial.println(mfrc522.GetStatusCodeName(status));
          return;
    }
    status = mfrc522.MIFARE_Read(blockAddr5, buffer1, &len);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("Reading failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }
    for (uint8_t i = 0; i < 16; i++)
    {
      if (buffer1[i] != 32)
      {
        Serial.write(buffer1[i]);
      }
    }

      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();

    }
    


