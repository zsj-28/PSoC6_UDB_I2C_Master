#include <Wire.h>

#define SLAVE_ADDRESS 0x27 // Use the same address as in your PSoC code

void setup() {
  // Initialize serial communication at 9600 bps:
  Serial.begin(9600);

  // Enable internal pull-ups on SDA and SCL
  pinMode(A4, INPUT_PULLUP);
  pinMode(A5, INPUT_PULLUP);

  // Initialize I2C as slave with the specified address
  Wire.begin(SLAVE_ADDRESS);

  // Register a function to be called when data is received
  Wire.onReceive(receiveData);

  Serial.println("I2C Slave ready.");
}

void loop() {
  // Main loop does nothing, all action is in receiveData
}

// Function to execute when data is received from master
void receiveData(int byteCount) {
  Serial.print("Received data: ");
  while (Wire.available()) {
    char c = Wire.read(); // Receive byte as a character
    Serial.print(c); // Print the character
  }
  Serial.println(); // Newline for readability
}
