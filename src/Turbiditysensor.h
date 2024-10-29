#ifndef TURBIDITYSENSOR_H
#define TURBIDITYSENSOR_H

#include <Arduino.h>

class TurbiditySensor {
public:
    // Constructor: Initialiseer met de pin waarop de sensor is aangesloten
    TurbiditySensor(int pin);

    // Functie om sensor te kalibreren in helder water
    void calibrate();

    // Functie om de NTU-waarde te berekenen
    float getNTU();

    // Functie om de gemeten spanning te krijgen
    float getVoltage();

private:
    int sensorPin;
    float clearWaterVoltage;
    float knownNTU;
    float knownNTUVoltage;

    // Functie om de spanning naar NTU om te zetten
    float mapVoltageToNTU(float voltage);
};

#endif
