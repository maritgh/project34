#include <Servo.h>
#include <Arduino.h>
 
// Motor A connections (€50 dispenser)
int enA = 9; // Enable pin for motor A
int in1 = 8; // Control pin 1 for motor A
int in2 = 7; // Control pin 2 for motor A
 
// Motor B connections (€20 dispenser)
int enB = 3; // Enable pin for motor B
int in3 = 4; // Control pin 1 for motor B
int in4 = 5; // Control pin 2 for motor B
 
// Servo connections
int servoPin50 = 10; // Servo pin for the €50 dispenser
int servoPin20 = 12; // Servo pin for the €20 dispenser
 
const int IRSensorPin = 11; // IR sensor pin to check if cash is dispensed
 
// Variables for dispenser counts and bank budget
int dispenser50Count = 13;
int dispenser20Count = 20;
int bankBudget = 1040;
bool transactionInProgress = false; // Flag to indicate if a transaction is in progress
 
Servo Servo50;
Servo Servo20;
 
void setup() {
  // Initialize the servos
  Servo50.attach(servoPin50);
  Servo50.write(0); // Initialize the €50 dispenser servo to the start position
 
  Servo20.attach(servoPin20);
  Servo20.write(0); // Initialize the €20 dispenser servo to the start position
 
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
 
  analogWrite(enA, 255); // Ensure motor A is initially off
  analogWrite(enB, 255); // Ensure motor B is initially off
 
  // Initialize the IR sensor
  pinMode(IRSensorPin, INPUT);
 
  // Initialize serial communication
  Serial.begin(9600);
  Serial.println("Enter the withdrawal amount (e.g., '50 1' for one €50 note or '20 2' for two €20 notes):");
}
 
void loop() {
  // Check for user commands only if no transaction is in progress
  if (!transactionInProgress && Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim(); // Remove leading and trailing whitespace, including the newline character
    Serial.println("Received command: " + command); // Debug message
    executeCommand(command); // Process the command
  }
}
 
// Function to execute the user command
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
      withdraw50(count); // Withdraw €50 notes
    } else if (denomination == "20") {
      withdraw20(count); // Withdraw €20 notes
    } else {
      Serial.println("Invalid denomination.");
      return;
    }
  }
}
 
// Function to withdraw €50 notes
void withdraw50(int count) {
  Serial.println("Withdraw €50 notes: " + String(count));
  for (int i = 0; i < count; i++) {
    bool dispensed = false;
    while (!dispensed) {
      // Move the servo to 180 degrees to dispense the note
      Servo50.write(180);
 
      // Turn on motor A
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
 
      // Set the transaction in progress
      transactionInProgress = true;
 
      // Wait for the dispensing to complete
      delay(2000); // Adjust this delay as necessary
 
      // Check if cash is dispensed
      if (digitalRead(IRSensorPin) == HIGH) { // HIGH means no cash
        Serial.println("Cash not detected, retrying...");
 
 
        Servo50.write(0); // Move the servo back to 0 degrees
        delay(500); // Delay before retrying
        Servo50.write(180);
      } else {
        // Turn off motor A
        digitalWrite(in1, LOW);
        digitalWrite(in2, LOW);
 
        Servo50.write(0); // Move the servo back to 0 degrees
        dispensed = true;
      }
 
      // Add a delay between dispensing each note, adjust as necessary
      delay(500); // 0.5 second delay between dispensing each note
    }
  }
 
  // Set the transaction as complete for €50 notes
  transactionInProgress = false;
}
 
// Function to withdraw €20 notes
void withdraw20(int count) {
  Serial.println("Withdraw €20 notes: " + String(count));
  for (int i = 0; i < count; i++) {
    bool dispensed = false;
    while (!dispensed) {
      // Move the servo to 180 degrees to dispense the note
      Servo20.write(180);
 
      // Turn on motor B
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
 
      // Set the transaction in progress
      transactionInProgress = true;
 
      // Wait for the dispensing to complete
      delay(2000); // Adjust this delay as necessary
 
      // Check if cash is dispensed
      if (digitalRead(IRSensorPin) == HIGH) { // HIGH means no cash
        Serial.println("Cash not detected, retrying...");
        // Turn off motor B
        digitalWrite(in3, LOW);
        digitalWrite(in4, LOW);
 
        Servo20.write(0); // Move the servo back to 0 degrees
        delay(500); // Delay before retrying
        Servo20.write(180);
      } else {
        // Turn off motor B
        digitalWrite(in3, LOW);
        digitalWrite(in4, LOW);
 
        Servo20.write(0); // Move the servo back to 0 degrees
        dispensed = true;
      }
 
      // Add a delay between dispensing each note, adjust as necessary
      delay(500); // 0.5 second delay between dispensing each note
    }
  }
 
  // Set the transaction as complete for €20 notes
  transactionInProgress = false;
}