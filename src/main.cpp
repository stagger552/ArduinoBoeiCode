/*******************************************************************************
 * Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman
 * Copyright (c) 2018 Terry Moore, MCCI
 *
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 *
 * This example sends a valid LoRaWAN packet with payload "Hello,
 * world!", using frequency and encryption settings matching those of
 * the The Things Network.
 *
 * This uses OTAA (Over-the-air activation), where where a DevEUI and
 * application key is configured, which are used in an over-the-air
 * activation procedure where a DevAddr and session keys are
 * assigned/generated for use with all further communication.
 *
 * Note: LoRaWAN per sub-band duty-cycle limitation is enforced (1% in
 * g1, 0.1% in g2), but not the TTN fair usage policy (which is probably
 * violated by this sketch when left running for longer)!

 * To use this sketch, first register your application and device with
 * the things network, to set or generate an AppEUI, DevEUI and AppKey.
 * Multiple devices can use the same AppEUI, but each device has its own
 * DevEUI and AppKey.
 *
 * Do not forget to define the radio type correctly in
 * arduino-lmic/project_config/lmic_project_config.h or from your BOARDS.txt.
 *
 *******************************************************************************/
#include <Arduino.h>

#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <DS18B20Sensor.h>
#include "Turbiditysensor.h"
#include <phMeter.h>
#include "GPSModule.h"
#include <Adafruit_NeoPixel.h>

#define LED_PIN 25
#define NUM_PIXELS 12 // Change this to match the number of LEDs in your strip

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

//
// For normal use, we require that you edit the sketch to replace FILLMEIN
// with values assigned by the TTN console. However, for regression tests,
// we want to be able to compile these scripts. The regression tests define
// COMPILE_REGRESSION_TEST, and in that case we define FILLMEIN to a non-
// working but innocuous value.
//
#ifdef COMPILE_REGRESSION_TEST
#define FILLMEIN 0
#else
#define FILLMEIN (#dont edit this, edit the lines that use FILLMEIN)
#endif

// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.
static const u1_t PROGMEM APPEUI[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
void os_getArtEui(u1_t *buf) { memcpy_P(buf, APPEUI, 8); }

// This should also be in little endian format, see above.
static const u1_t PROGMEM DEVEUI[8] = {0x73, 0xB2, 0x06, 0xD0, 0x7E, 0xD5, 0xB3, 0x70};
void os_getDevEui(u1_t *buf) { memcpy_P(buf, DEVEUI, 8); }

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from ttnctl can be copied as-is.
static const u1_t PROGMEM APPKEY[16] = {0x45, 0x80, 0xAC, 0xAA, 0xE6, 0xF7, 0x57, 0x7A, 0x9A, 0x8A, 0x3F, 0xEC, 0x5A, 0x4D, 0xB4, 0x1E};
void os_getDevKey(u1_t *buf) { memcpy_P(buf, APPKEY, 16); }

static uint8_t mydata[18];
static osjob_t sendjob;

void do_send(osjob_t *);

// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
const unsigned TX_INTERVAL = 20;

// Pin mapping TTGO LoRa32 V1.0:
const lmic_pinmap lmic_pins = {
    .nss = LORA_CS,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = LORA_RST,
    .dio = {LORA_IRQ, A5, A4},
};

void printHex2(unsigned v)
{
    Serial.print("0123456789ABCDEF"[v >> 4]);
    Serial.print("0123456789ABCDEF"[v & 0xF]);
}

void onEvent(ev_t ev)
{
    String receivedCommand = "";
bool SEND_DATA = false;

    Serial.print(os_getTime());
    Serial.print(": ");
    switch (ev)
    {
    case EV_SCAN_TIMEOUT:
        Serial.println(F("EV_SCAN_TIMEOUT"));
        break;
    case EV_BEACON_FOUND:
        Serial.println(F("EV_BEACON_FOUND"));
        break;
    case EV_BEACON_MISSED:
        Serial.println(F("EV_BEACON_MISSED"));
        break;
    case EV_BEACON_TRACKED:
        Serial.println(F("EV_BEACON_TRACKED"));
        break;
    case EV_JOINING:
        Serial.println(F("EV_JOINING"));
        break;

    case EV_JOINED:
        Serial.println(F("EV_JOINED"));
        {
            u4_t netid = 0;
            devaddr_t devaddr = 0;
            u1_t nwkKey[16];
            u1_t artKey[16];
            LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
            Serial.print("netid: ");
            Serial.println(netid, DEC);
            Serial.print("devaddr: ");
            Serial.println(devaddr, HEX);
            Serial.print("AppSKey: ");
            for (size_t i = 0; i < sizeof(artKey); ++i)
            {
                if (i != 0)
                    Serial.print("-");
                printHex2(artKey[i]);
            }
            Serial.println("");
            Serial.print("NwkSKey: ");
            for (size_t i = 0; i < sizeof(nwkKey); ++i)
            {
                if (i != 0)
                    Serial.print("-");
                printHex2(nwkKey[i]);
            }
            Serial.println();
        }
        // Disable link check validation (automatically enabled
        // during join, but because slow data rates change max TX
        // size, we don't use it in this example.
        LMIC_setLinkCheckMode(0);
        break;
    /*
    || This event is defined but not used in the code. No
    || point in wasting codespace on it.
    ||
    || case EV_RFU1:
    ||     Serial.println(F("EV_RFU1"));
    ||     break;
    */
    case EV_JOIN_FAILED:
        Serial.println(F("EV_JOIN_FAILED"));
        break;
    case EV_REJOIN_FAILED:
        Serial.println(F("EV_REJOIN_FAILED"));
        break;
    case EV_TXCOMPLETE:
        Serial.print(F("Received "));
        Serial.print(LMIC.dataLen);
        Serial.println(F(" bytes of payload"));

        // Print the raw payload
        Serial.print(F("Payload: "));
        for (int i = 0; i < LMIC.dataLen; i++)
        {
            Serial.print((char)LMIC.frame[LMIC.dataBeg + i]); // Print as characters
        }
        Serial.println();

        for (int i = 0; i < LMIC.dataLen; i++)
        {
            receivedCommand += (char)LMIC.frame[LMIC.dataBeg + i];
        }

        if (receivedCommand == "START")
        {
            startSetup();
        }

        if (receivedCommand == "TRIGGERDATA")
        {
            if (SEND_DATA == true)
            {
                SEND_DATA = false;
            }
            if (SEND_DATA == false)
            {
                SEND_DATA = true;
            }
            
            Serial.print(F("SEND_DATA toggled to: "));
            Serial.println(SEND_DATA);
        }

        if (receivedCommand == "KNIPPEREN")
        {
            blinkRGB();
        }
        break;
    case EV_LOST_TSYNC:
        Serial.println(F("EV_LOST_TSYNC"));
        break;
    case EV_RESET:
        Serial.println(F("EV_RESET"));
        break;
    case EV_RXCOMPLETE:
        // data received in ping slot
        Serial.println(F("EV_RXCOMPLETE"));
        break;
    case EV_LINK_DEAD:
        Serial.println(F("EV_LINK_DEAD"));
        break;
    case EV_LINK_ALIVE:
        Serial.println(F("EV_LINK_ALIVE"));
        break;
    /*
    || This event is defined but not used in the code. No
    || point in wasting codespace on it.
    ||
    || case EV_SCAN_FOUND:
    ||    Serial.println(F("EV_SCAN_FOUND"));
    ||    break;
    */
    case EV_TXSTART:
        Serial.println(F("EV_TXSTART"));
        break;
    case EV_TXCANCELED:
        Serial.println(F("EV_TXCANCELED"));
        break;
    case EV_RXSTART:
        /* do not print anything -- it wrecks timing */
        break;
    case EV_JOIN_TXCOMPLETE:
        Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
        break;

    default:
        Serial.print(F("Unknown event: "));
        Serial.println((unsigned)ev);
        break;
    }
}

void do_send(osjob_t *j)
{
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND)
    {
        Serial.println(F("OP_TXRXPEND, not sending"));
    }
    else
    {
        // PUT HERE YOUR CODE TO READ THE SENSORS AND CONSTRUCT THE TTS PAYLOAD
        const uint8_t ONE_WIRE_BUS = 13;
        TurbiditySensor turbSensor(12);
        const uint8_t PH_SENSOR_PIN = 15;
        const int oxygenSensorPin = 27;

        // Create an instance of DS18B20Sensor
        DS18B20Sensor temperatureSensor(ONE_WIRE_BUS);
        int analogValue = analogRead(PH_SENSOR_PIN);

        temperatureSensor.begin();
        float tempC = temperatureSensor.getTemperatureC();
        Serial.print("Temperature in Celsius: ");
        Serial.println(tempC);

        Serial.println("Calibrating Turbidity Sensor");
        turbSensor.calibrate();

        float ntu_val = turbSensor.getNTU();
        float volt = turbSensor.getVoltage();

        Serial.print("Nephelometric Turbidity Units (NTU): ");
        Serial.println(ntu_val);
        Serial.print("Turbidity voltage: ");
        Serial.println(volt);

        PHMeter phMeter(PH_SENSOR_PIN, tempC);
        float pHValue = phMeter.readPH();

        GPSModule gpsModule(16, 17);
        gpsModule.begin(9600);
        float latitude = 0;
        float longitude = 0;

        if (gpsModule.update())
        {
            latitude = gpsModule.getLatitude();
            longitude = gpsModule.getLongitude();
        }

        gpsModule.checkSignalTimeout(5000);

        // Output the pH value to the Serial Monitor
        Serial.print("pH Value: ");
        Serial.println(pHValue);

        // Zet temperatuur om naar 2 bytes (signed integer, x100 voor precisie)
        int16_t temp = (int16_t)(tempC * 100);
        mydata[0] = temp >> 8;   // MSB
        mydata[1] = temp & 0xFF; // LSB

        // Zet NTU om naar 2 bytes (signed integer, x100 voor precisie)
        int16_t ntu = (int16_t)(ntu_val * 100);
        mydata[2] = ntu >> 8;
        mydata[3] = ntu & 0xFF;

        // Zet pH-waarde om naar 2 bytes (signed integer, x100 voor precisie)
        int16_t pH = (int16_t)(pHValue * 100);
        mydata[4] = pH >> 8;
        mydata[5] = pH & 0xFF;

        int32_t lat = (int32_t)(latitude * 1000000);
        mydata[8] = (lat >> 24) & 0xFF;
        mydata[9] = (lat >> 16) & 0xFF;
        mydata[10] = (lat >> 8) & 0xFF;
        mydata[11] = lat & 0xFF;

        // Zet lengtegraad om naar 4 bytes (vermenigvuldigd met 1 miljoen)
        int32_t lon = (int32_t)(longitude * 1000000);
        mydata[12] = (lon >> 24) & 0xFF;
        mydata[13] = (lon >> 16) & 0xFF;
        mydata[14] = (lon >> 8) & 0xFF;
        mydata[15] = lon & 0xFF;

        // Print the payload to the console
        Serial.print("Payload: ");
        for (size_t i = 0; i < sizeof(mydata) - 1; ++i)
        {
            if (i != 0)
                Serial.print("-");
            printHex2(mydata[i]);
        }
        Serial.println();

        // Prepare upstream data transmission at the next possible time.
        LMIC_setTxData2(1, mydata, sizeof(mydata), 0);
        Serial.println(F("Packet queued"));
    }
    // Next TX is scheduled after TX_COMPLETE event.
}

void blinkRGB()
{
    int durationMs = 30000; // Standard duration: 30 seconds
    int intervalMs = 500;   // Standard blinking interval: 500 ms
    int startTime = millis();
    uint32_t colors[] = {strip.Color(255, 0, 0), strip.Color(0, 255, 0), strip.Color(0, 0, 255)}; // RGB
    int colorIndex = 0;

    while (millis() - startTime < durationMs)
    {
        for (int i = 0; i < NUM_PIXELS; i++)
        {
            strip.setPixelColor(i, colors[colorIndex]); // Set the current color
        }
        strip.show();
        delay(intervalMs); // Wait for the interval duration
        strip.clear();     // Turn off all LEDs
        strip.show();
        delay(intervalMs);                 // Wait again
        colorIndex = (colorIndex + 1) % 3; // Cycle to the next color
    }

    strip.clear(); // Turn off all LEDs after the duration
    strip.show();
}

void blinkSetup()
{
    int durationMs = 10000; // Blink for 10 seconds
    int intervalMs = 500;   // Blinking interval (500 ms on/off)
    int startTime = millis();
    uint32_t green = strip.Color(0, 255, 0); // Green color

    while (millis() - startTime < durationMs)
    {
        for (int i = 0; i < NUM_PIXELS; i++)
        {
            strip.setPixelColor(i, green); // Set all LEDs to green
        }
        strip.show();
        delay(intervalMs); // Wait for the interval

        strip.clear(); // Turn off all LEDs
        strip.show();
        delay(intervalMs); // Wait again
    }

    strip.clear(); // Ensure LEDs are off after the duration
    strip.show();
}
void setup()
{
    Serial.begin(9600);
    Serial.println(F("Starting"));

    // Initialize the NeoPixel strip
    strip.begin();            // Start de NeoPixel
    strip.show();             // Alle LEDs uit
    strip.setBrightness(255); // Pas helderheid aan (0-255)
}
void checkDownlink(String Command)
{

    if (Command == "Start")
    {
    }
    if (Command == "DataTrigger")
    {
    }
    if (Command == "Knipper")
    {
    }
}

void startSetup()
{
    blinkSetup();
    blinkRGB();

    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();

}
void loop()
{
    os_runloop_once();
}
