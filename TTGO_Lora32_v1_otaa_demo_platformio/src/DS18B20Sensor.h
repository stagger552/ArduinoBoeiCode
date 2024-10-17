#ifndef DS18B20SENSOR_H
#define DS18B20SENSOR_H

#include <OneWire.h>
#include <DallasTemperature.h>

class DS18B20Sensor {
  public:
    // Constructor
    DS18B20Sensor(uint8_t pin);

    // Function to begin communication
    void begin();

    // Function to get temperature in Celsius
    float getTemperatureC();

    // Function to get temperature in Fahrenheit
    float getTemperatureF();

  private:
    OneWire oneWire;
    DallasTemperature sensors;
};

#endif
