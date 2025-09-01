/*
  Arduino Door Lock System
  -------------------------
  - Uses a keypad to enter a password
  - Shows messages on an LCD screen
  - Unlocks and locks a door using a servo motor
  - LED indicator turns ON when the door is open
*/

#include <Keypad.h>        // Library to read the keypad
#include <LiquidCrystal.h> // Library to control the LCD
#include <Servo.h>         // Library to move the servo motor

Servo myservo; // Servo object for the door lock

// Setup LCD pins (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

// Password setup
#define Password_Length 5   // 4 digits + 1 null character at the end
char Data[Password_Length];       // Stores the password entered
char Master[Password_Length] = "1234"; // Correct password

int pos = 0;              // To keep track of servo position
byte data_count = 0;      // How many digits have been entered
char customKey;           // Stores which key was pressed
bool door = true;         // Door starts as closed

// Keypad layout (4 rows x 3 columns)
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

// Connect keypad pins to Arduino
byte rowPins[ROWS] = {1, 2, 3, 4};
byte colPins[COLS] = {5, 6, 7};

// Make the keypad object
Keypad customKeypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  pinMode(10, OUTPUT);     // Pin 10 for LED (door status)
  myservo.attach(9);       // Servo connected to pin 9
  ServoClose();            // Keep door locked at startup

  // Show welcome message
  lcd.begin(16, 2);
  lcd.print(" Arduino Door");
  lcd.setCursor(0, 1);
  lcd.print("--Look project--");
  delay(3000);
  lcd.clear();
}

void loop() {
  // If door is open, wait for the close command
  if (door == 0) {
    customKey = customKeypad.getKey();
    if (customKey == '#') {    // Press "#" to close the door
      lcd.clear();
      ServoClose();
      lcd.print("  Door is Close");
      digitalWrite(10, LOW);   // Turn LED OFF
      delay(3000);
      door = 1;                // Mark door as closed
    }
  } 
  else {
    // If door is closed, wait for a password
    Open();
  }
}

// Reset password input
void clearData() {
  while (data_count != 0) {
    Data[data_count--] = 0;  
  }
}

// Open the servo slowly (unlock door)
void ServoOpen() {
  for (pos = 180; pos >= 0; pos -= 5) {
    myservo.write(pos);
    delay(15);
  }
}

// Close the servo slowly (lock door)
void ServoClose() {
  for (pos = 0; pos <= 180; pos += 5) {
    myservo.write(pos);
    delay(15);
  }
}

// Ask user for password and check it
void Open() {
  lcd.setCursor(0, 0);
  lcd.print(" Enter Password");
  
  customKey = customKeypad.getKey();
  
  if (customKey) {
    // Save the entered digit
    Data[data_count] = customKey;  
    lcd.setCursor(data_count, 1);
    lcd.print(Data[data_count]);   // Show it on screen
    data_count++;
  }

  // If 4 digits are entered, check the password
  if (data_count == Password_Length - 1) {
    if (!strcmp(Data, Master)) {   // If password matches
      lcd.clear();
      digitalWrite(10, HIGH);      // Turn LED ON
      ServoOpen();
      lcd.print("  Door is Open");
      door = 0;                    // Mark door as open
    } 
    else {                         // Wrong password
      lcd.clear();
      lcd.print(" Wrong Password");
      delay(1000);
      door = 1;                    // Keep door closed
    }
    clearData(); // Reset input
  }
}
