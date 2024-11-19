#include "DS18B20Sensor.h"

DS18B20Sensor::DS18B20Sensor(uint8_t pin) : oneWire(pin), sensors(&oneWire) {
}

void DS18B20Sensor::begin() {
  sensors.begin();
}

float DS18B20Sensor::getTemperatureC() {
  sensors.requestTemperatures(); 
  return sensors.getTempCByIndex(0); 
}

float DS18B20Sensor::getTemperatureF() {
  sensors.requestTemperatures(); 
  return sensors.getTempFByIndex(0);
}
