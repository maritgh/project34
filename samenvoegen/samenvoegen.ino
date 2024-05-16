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
void keypadpress(){
  char Key;
  if (Key = KP2.Getkey()) {
      Serial << "Key: " << Key << "\n";
      lcd.setCursor(0, 0);
      lcd.clear();
      lcd.print(Key);
    }
}
void buttonpress(int value_key){
  Serial.println("Keypressed");
  if (value_key > 850) {
    Serial.println("Button6");
  } else if (value_key > 800) {
    Serial.println("Button5");
  } else if (value_key > 700) {
    Serial.println("Button4");
  } else if (value_key > 600) {
    Serial.println("Button3");
  } else if (value_key > 450) {
    Serial.println("Button2");
  } else if (value_key < 450) {
    Serial.println("Button1");
  } else{
    Serial.println("Error");
  }
   delay(500);
}