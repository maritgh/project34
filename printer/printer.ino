#include "Adafruit_Thermal.h"
#include "SoftwareSerial.h"
#include <DS3231.h>
 
#define TX_PIN 6
#define RX_PIN 5
 
SoftwareSerial mySerial(RX_PIN, TX_PIN);
Adafruit_Thermal printer(&mySerial);
 
void setup() {
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
 
  mySerial.begin(9600);
  printer.begin();
 
  delay(500); // Give the printer some time to initialize
 
  // Print the header
  printer.doubleHeightOn();
  printer.println(F("RTB"));
  printer.doubleHeightOff();
  delay(200); // Increased delay
  
  // Print date and time
  printer.justify('C');
  printer.println(F("Date"));
  delay(200); // Increased delay
  printer.justify('C');
  printer.println(F("Time"));
  delay(200); // Increased delay
 
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
  delay(200); // Increased delay
 
  // Print gepind bedrag with smaller font size
  printer.setFont(1); // Set smaller font size
  printer.justify('C');
  printer.println(F("Pinned"));
  printer.underlineOn();
  printer.setFont(1); // Set smaller font size
  printer.justify('C');
  printer.println(F("Amount"));
  printer.underlineOn();
  delay(200);
 
  printer.println(F("100,00"));
  printer.underlineOff();
  printer.setFont(0); // Revert to the original font size
  delay(200); // Increased delay
 
  // Print thank you message
  // Print thank you message
printer.justify('C');
printer.println(F("Thank\nyou"));
printer.setLineHeight(50);
 
printer.println(F("Raccoon"));
 
printer.justify('C');
printer.println(F("Trust &\nBank"));
 
}
 
void loop() {
  // Do nothing in the loop function
}