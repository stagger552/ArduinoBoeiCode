#include "PhMeter.h"

// Constructor: Initialize the pin
// Constructor: Initialize the pin
// Constructor: Initialize the pin
// Constructor: Initialize the pin and temperature
PHMeter::PHMeter(uint8_t pin, float temp) : phPin(pin), temperature(temp) {
    // Nothing to do here
}
// Initialize the pH sensor
void PHMeter::begin() {
    PhMeter.begin();         // Initialize the pH meter object
}

// Read and return the pH value
// Read and return the pH value
float PHMeter::readPH() {
    int analogValue = analogRead(phPin);          // Read the analog value
    float voltage = analogValue * (5.0 / 1023.0); // Convert analog value to voltage
    return PhMeter.readPH(voltage, temperature);  // Read pH using the stored temperature
}