
int thresholds[16] = {631, 718, 790, 253, 655, 735, 798, 340, 703, 765, 817, 488, 727, 780, 827, 550};
char keypad[16] = {'1','2','3','A','4','5','6','B','7','8','9','C','*','0','#','D'};
int range = 4;
// const int numReadings = 50;     // gemiddelde van 50 lezingen om ruis te verminderen
// int readings[numReadings];
// int readIndex = 0; 
// int total = 0; 
// int val = 0;

void setup() {
  pinMode(A0, INPUT);
  // put your setup code here, to run once:
  Serial.begin(9600);
  //   for (int thisReading = 0; thisReading < numReadings; thisReading++) {   // alle lezingen op 0 voor het starten
  //   readings[thisReading] = 0;
  // }
}

void loop() {
  // total = total - readings[readIndex];    // gemiddelde van de lezingen berekenen
  // readings[readIndex] = analogRead(A0);
  // total = total + readings[readIndex];
  // readIndex = readIndex + 1;

  // if (readIndex >= numReadings) {
  //   readIndex = 0;
  // }
  Serial.println(analogRead(A0));
  // val = total / numReadings;
  int key = analogRead(A0);
  for(int i = 0; i < 16; i++){
    if(key >= thresholds[i] - range && key <= thresholds[i] + range){
      Serial.println(keypad[i]);
    }
  }
  if(analogRead(A0) != 1023){
  }
  
}
// #include <ezAnalogKeypad.h>

// ezAnalogKeypad keypad(A0);  // create ezAnalogKeypad object that attach to pin A0

// void setup() {
//   Serial.begin(9600);
//   setNoPressValue(1023);

//   // MUST READ: You MUST run the calibration example, press key one-by-one to get the analog values
//   // The below values is just an example, your keypad's value may be different
//   keypad.setNoPressValue(0);  // analog value when no key is pressed
//   keypad.registerKey('1', 631); // analog value when the key '1' is pressed
//   keypad.registerKey('2', 718); // analog value when the key '2' is pressed
//   keypad.registerKey('3', 790); // analog value when the key '3' is pressed
//   keypad.registerKey('A', 253);
//   keypad.registerKey('4', 655); // analog value when the key '4' is pressed
//   keypad.registerKey('5', 735);
//   keypad.registerKey('6', 798);
//   keypad.registerKey('B', 340);
//   keypad.registerKey('7', 703);
//   keypad.registerKey('8', 765);
//   keypad.registerKey('9', 817);
//   keypad.registerKey('C', 488);
//   keypad.registerKey('*', 727); // analog value when the key '*' is pressed
//   keypad.registerKey('0', 780); // analog value when the key '0' is pressed
//   keypad.registerKey('#', 827); // analog value when the key '#' is pressed
//   keypad.registerKey('D', 550);
//   // ADD MORE IF YOUR KEYPAD HAS MORE
// }

// void loop() {
//   unsigned char key = keypad.getKey();
//   if (key) {
//     Serial.println(key);
//   }
// }