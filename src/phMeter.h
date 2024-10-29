#ifndef PH_METER_H
#define PH_METER_H

#include <Arduino.h>     // Needed for types and functions like Serial
#include <DFRobot_PH.h>  // Include pH sensor library
#include <EEPROM.h>      // Include EEPROM library

class PHMeter {
public:
    PHMeter(uint8_t pin, float temp); // Constructor with default temperature
    void begin();
    float readPH();

private:
    DFRobot_PH PhMeter;   // DFRobot_PH sensor object
    uint8_t phPin;         // Analog pin for pH sensor
    float temperature;     // Temperature for pH calculation
};

#endif
