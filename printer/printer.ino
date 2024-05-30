#include <Adafruit_Thermal.h>
#include "SoftwareSerial.h"
//#include <AltSoftSerial.h>
#include <DS3231.h>
 
#define TX_PIN 19
#define RX_PIN 18

//AltSoftSerial mySerial(RX_PIN, TX_PIN);
SoftwareSerial mySerial(RX_PIN, TX_PIN);
Adafruit_Thermal printer(&mySerial);
 
void setup() {
  // pinMode(7, OUTPUT);
  // digitalWrite(7, LOW);
 
  mySerial.begin(9600);
  Serial.begin(9600);
  printer.begin();
  printer.setHeatConfig(11, 200, 90);
 printer.sleep();
   printbon();
  delay(1000);
}
 
void loop() {

}
void printbon(){
  printer.wake();

  delay(1000); // Give the printer some time to initialize
  printer.println("\n");
  printer.justify('L');
  printer.setLineHeight(30);

  // Print the header
  printer.doubleHeightOn();
  printer.println(F("RTB"));
  printer.doubleHeightOff();
  delay(500); // Increased delay
  
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