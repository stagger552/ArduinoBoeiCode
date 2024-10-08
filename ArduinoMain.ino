#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>

// Replace with your TTN credentials
static const u1_t PROGMEM DEVEUI[8] = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x06, 0xAE, 0x7D }; // DevEUI
static const u1_t PROGMEM APPEUI[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };         // Replace with your AppEUI
static const u1_t PROGMEM APPKEY[16] = { 0x8D, 0xB8, 0x04, 0xA4, 0x4E, 0x3F, 0xC3, 0xFB,
                                         0x49, 0xDF, 0x16, 0xC0, 0x77, 0xE8, 0xEE, 0xD };  // AppKey



// Device-specific configuration
const lmic_pinmap lmic_pins = {
  .nss = 10,  // Chip Select pin
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 9,                          // Reset pin
  .dio = { 2, 3, LMIC_UNUSED_PIN },  // DIO0, DIO1 pins
};
// const char message[] = "Hello, LoRa!"; 
const char message[] = "Hello, LoRa!"; // Example message

void setup() {

  Serial.begin(9600);
  Serial.println(F("Starting..."));

  // LMIC init
  os_init();
  LMIC_reset();


  // Set up the LMIC with your credentials
  LMIC_setSession(0x1, DEVEUI, APPKEY, APPEUI);
  LMIC_selectSubBand(1);        // Select sub-band (0-7 for EU868)
  LMIC_setDrTxpow(DR_SF7, 14);  // Set data rate and transmit power
}

void loop() {
  // Prepare and send the data
  Serial.println(F("Sending..."));
  // LMIC_setTxData2(1, (uint8_t*)message, strlen(message), 0);
  
  delay(10000);  // Wait 10 seconds before sending again

  // Process events
  os_runloop_once();
  // put your main code here, to run repeatedly:
}

// Function to handle LoRaWAN events
void onEvent(ev_t ev) {
  switch (ev) {
    case EV_JOINING:
      Serial.println(F("Joining..."));
      break;
    case EV_JOINED:
      Serial.println(F("Joined"));
      // LMIC_setTxData2(1, (uint8_t*)message, strlen(message), 0);
      break;
    case EV_TXCOMPLETE:
      Serial.println(F("TX Complete"));
      break;
    default:
      Serial.println(F("Unknown event"));
      break;
  }
}
