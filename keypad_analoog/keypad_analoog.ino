#include <OnewireKeypad.h>
#include <LiquidCrystal.h>
 
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
}
 
void loop() {
  char Key;
  byte KState = KP2.Key_State();
  if (KState == PRESSED) {
    if (Key = KP2.Getkey()) {
      
      Serial << "Key: " << Key << "\n";
      lcd.setCursor(0, 0);
      lcd.clear();
      lcd.print(Key);
    }
  }
}
