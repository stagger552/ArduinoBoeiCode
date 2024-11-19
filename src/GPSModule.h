// GPSModule.h
#ifndef GPSMODULE_H
#define GPSMODULE_H

#include <TinyGPS++.h>
#include <SoftwareSerial.h>

class GPSModule {
public:
    // Constructor for GPSModule, specifying RX and TX pins
    GPSModule(int rxPin, int txPin);

    // Initialize the GPS module with the given baud rate
    void begin(long baudRate);

    // Update GPS data; returns `true` if location data has been updated
    bool update();

    // Print the current location to the serial monitor (if available)
    void printLocation();

    // Check if there has been no new GPS data within the given timeout
    void checkSignalTimeout(unsigned long timeout);

    // Get the current latitude
    double getLatitude() { return gps.location.lat(); }

    // Get the current longitude
    double getLongitude() { return gps.location.lng(); }

private:
    TinyGPSPlus gps;           // GPS data processing
    SoftwareSerial gpsSerial;   // SoftwareSerial object for GPS communication
};

#endif // GPSMODULE_H