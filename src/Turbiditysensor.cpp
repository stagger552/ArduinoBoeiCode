#include "Turbiditysensor.h"

// Constructor: stelt de sensorpin en kalibratie-instellingen in
TurbiditySensor::TurbiditySensor(int pin)
    : sensorPin(pin), clearWaterVoltage(4.1), knownNTU(100), knownNTUVoltage(2.5) {}

// Kalibreer de sensor door de spanning in helder water op te slaan
void TurbiditySensor::calibrate() {
    clearWaterVoltage = getVoltage();
    Serial.print("Clear water voltage: ");
    Serial.println(clearWaterVoltage);
}

// Lees de spanning van de sensor
float TurbiditySensor::getVoltage() {
    int sensorValue = analogRead(sensorPin);
    float voltage = sensorValue * (5.0 / 1023.0); // Veronderstelt 5V Arduino
    return voltage;
}

// Bereken NTU op basis van de gemeten spanning
float TurbiditySensor::getNTU() {
    float voltage = getVoltage();
    return mapVoltageToNTU(voltage);
}

// Functie om spanning naar NTU te mappen op basis van kalibratie
float TurbiditySensor::mapVoltageToNTU(float voltage) {
    float ntu = (knownNTU / (clearWaterVoltage - knownNTUVoltage)) * (clearWaterVoltage - voltage);
    return ntu;
}
