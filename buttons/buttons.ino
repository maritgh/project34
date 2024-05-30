void setup() {
  Serial.begin(9600);
}
 
void loop() {
  int value = analogRead(A2);
 
  // Measure raw analogRead values for each button
  // Serial.print("Analog Read Value: ");
   Serial.println(value);
 
  /*If two buttons are pressed at the time then lowest value is used*/
 
  // Adjust these ranges based on your observations
  if (value == 1023) {
    Serial.println("No button pressed");
  } else if (value > 900) {
    Serial.println("Button6");
  } else if (value > 850) {
    Serial.println("Button5");
  } else if (value > 800) {
    Serial.println("Button4");
  } else if (value > 600) {
    Serial.println("Button3");
  } else if (value > 450) {
    Serial.println("Button2");
  } else if (value < 450) {
    Serial.println("Button1");
  } else{
    Serial.println("Error");
  }
 
  delay(500);
}