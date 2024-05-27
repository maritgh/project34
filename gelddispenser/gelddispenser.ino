#include <Servo.h>
#include <Arduino.h>
 
// Motor A connections
int enA = 9; // Enable pin for motor A (connected to L293D ENA pin)
int in1 = 8; // Control pin 1 for motor A (connected to L293D IN1 pin)
int in2 = 7; // Control pin 2 for motor A (connected to L293D IN2 pin)
int servoPin50 = 10; // Servo pin for the €50 dispenser
const int IRSensorPin = 11; // IR sensor pin to check if cash is dispensed
 
// variables for dispenser counts and bank budget
int dispenser50Count = 13;
int bankBudget = 1040;
 
Servo Servo50;
 
void setup() {
  // initialize the servo
  Servo50.attach(servoPin50);
  Servo50.write(0); // Move the servo to the end position
 
  // Initialization code for the motor driver
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  analogWrite(enA, 0); // Ensure the motor is initially off
 
  // Initialize the IR sensor
  pinMode(IRSensorPin, INPUT);
 
  // Initialize serial communication
  Serial.begin(9600);
  Serial.println("Enter the withdrawal amount (in multiples of 50):");
}
 
void loop() {
  // Check for user commands
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim(); // Remove leading and trailing whitespace, including the newline character
    Serial.println("Received command: " + command); // Debug message
    if (command.equals("RESET")) {
      resetATM(); // Reset the ATM if the user entered "RESET"
    } else {
      // Check if the input is a valid withdrawal amount
      int amount = command.toInt();
      if (amount % 50 == 0 && amount <= 500) {
        withdrawMoney(amount); // Withdraw money if the amount is valid
      } else {
        // Inform the user of invalid input
        Serial.println("Invalid amount. Enter a multiple of 50, up to €500, or 'RESET' to reset the ATM.");
      }
    }
  }
}
 
void resetATM() {
  // Reset dispenser counts and bank budget
  dispenser50Count = 13;
  bankBudget = 1040;
 
  // Inform the user of the reset
  Serial.println("The ATM has been reset. Dispenser counts set to 13 notes each. Bank budget set to €1040.");
}
 
void issueNote() {
  if (dispenser50Count > 0) {
    dispenser50Count--;  // Decrease only if the dispenser has notes
 
    bool noteDispensed = false;
    while (!noteDispensed) {
      // Code to dispense a €50 note
      Serial.println("Dispensing: €50");
 
      analogWrite(enA, 255); // Set the motor to maximum speed
      digitalWrite(in1, LOW); // Set the motor direction (adjusted for correct direction)
      digitalWrite(in2, HIGH);  // Set the motor direction (adjusted for correct direction)
      delay(100);
      Servo50.write(180); // Move the servo to the start position
      delay(500); // Wait 0.5 seconds to ensure the note is dispensed
      Servo50.write(0);
      delay(500);
      Servo50.write(180);
      delay(5000);
      analogWrite(enA, 0); // Stop the motor
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
      delay(1000); // Wait 1 second for stability
      Servo50.write(0); // Move the servo to the end position
      delay(1000); // Wait 1 second for the servo to move
 
      // Check if the IR sensor detects black (cash stuck)
      if (digitalRead(IRSensorPin) == LOW) { // LOW means no black detected, hence cash dispensed
        noteDispensed = true;
        Serial.println("Note successfully dispensed");
      } else {
        Serial.println("Cash stuck, trying again...");
      }
    }
  } else {
    Serial.println("The dispenser is empty.");
  }
}
 
// Function to withdraw money
void withdrawMoney(int amount) {
  // Calculate the total available amount in the ATM
  int totalAvailable = dispenser50Count * 50;
 
  // Check if there is enough money available for withdrawal
  if (amount <= totalAvailable) {
    // Calculate the number of €50 notes to be dispensed
    int count50 = amount / 50;
 
    // Dispense the €50 notes
    for (int i = 0; i < count50; i++) {
      issueNote();
    }
 
    // Update the bank budget after the successful transaction
    bankBudget -= (count50 * 50);
 
    // Print the dispenser counts and the bank budget for debugging
    Serial.print("Dispenser count 50: ");
    Serial.println(dispenser50Count);
    Serial.print("Bank budget: ");
    Serial.println(bankBudget);
 
    // Inform the user that the transaction was successful
    Serial.println("Transaction successful. Please take your money.");
  } else {
    // Inform the user that there is not enough money available in the ATM
    Serial.println("Insufficient funds available in the ATM.");
  }