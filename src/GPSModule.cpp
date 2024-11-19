// GPSModule.cpp
#include "GPSModule.h"

// Constructor: initialiseert SoftwareSerial op de opgegeven RX- en TX-pinnen
GPSModule::GPSModule(int rxPin, int txPin) : gpsSerial(rxPin, txPin) {}

// Initialiseer de GPS-module met de opgegeven baudrate
void GPSModule::begin(long baudRate) {
  gpsSerial.begin(baudRate);
  Serial.println("GPS-module gestart, wacht op GPS-signaal...");
}

// Update GPS-gegevens en retourneert `true` als er nieuwe locatiegegevens zijn
bool GPSModule::update() {
  bool updated = false;
  while (gpsSerial.available() > 0) {
    char c = gpsSerial.read();
    gps.encode(c);  // Verwerk de ontvangen GPS-data
    Serial.print(c);  // Optioneel: print ruwe NMEA-gegevens voor debugging

    // Als de locatie is bijgewerkt, stel updated in op `true`
    if (gps.location.isUpdated()) {
      updated = true;
    }
  }
  return updated;
}

// Print de huidige locatie (indien beschikbaar) naar de seriële monitor
void GPSModule::printLocation() {
  if (gps.location.isValid()) {
    Serial.print("Latitude: ");
    Serial.print(gps.location.lat(), 6);
    Serial.print(" Longitude: ");
    Serial.println(gps.location.lng(), 6);
  } else {
    Serial.println("Locatie niet beschikbaar.");
  }
}

// Controleert of er geen nieuwe GPS-gegevens zijn ontvangen binnen de gegeven timeout
void GPSModule::checkSignalTimeout(unsigned long timeout) {
  if (gps.location.age() > timeout) {  // age() geeft de tijd in ms sinds de laatste update
    Serial.println("Geen nieuwe GPS-gegevens ontvangen...");
  }
}
