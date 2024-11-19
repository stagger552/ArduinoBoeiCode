#ifndef DS18B20SENSOR_H
#define DS18B20SENSOR_H

#include <OneWire.h>
#include <DallasTemperature.h>

class DS18B20Sensor {
  public:
    DS18B20Sensor(uint8_t pin);

    
    void begin();

    
    float getTemperatureC();

    
    float getTemperatureF();

  private:
    OneWire oneWire;
    DallasTemperature sensors;
};

#endif
