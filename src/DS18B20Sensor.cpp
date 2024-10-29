#include "DS18B20Sensor.h"

// Constructor - initializes OneWire instance and DallasTemperature
DS18B20Sensor::DS18B20Sensor(uint8_t pin) : oneWire(pin), sensors(&oneWire) {
}

// Begin communication with DS18B20 sensor
void DS18B20Sensor::begin() {
  sensors.begin();
}

// Get temperature in Celsius
float DS18B20Sensor::getTemperatureC() {
  sensors.requestTemperatures(); // Request temperature from the sensor
  return sensors.getTempCByIndex(0); // Return temperature in Celsius
}

// Get temperature in Fahrenheit
float DS18B20Sensor::getTemperatureF() {
  sensors.requestTemperatures(); // Request temperature from the sensor
  return sensors.getTempFByIndex(0); // Return temperature in Fahrenheit
}
