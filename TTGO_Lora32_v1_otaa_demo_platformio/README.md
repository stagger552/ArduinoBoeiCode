# TTGO_otaa.platformio

This project can be used as startup project for a The Things Network software stack demo with a Lilygo TTGO Lora32 V1 in with the IDE plugin Platform.IO.

To be able to use all the TTS compatible libraries dependencies and build flags are added to the platform.io file 
```
lib_deps =
    mcci-catena/MCCI LoRaWAN LMIC library@^4.1.1

build_flags =
    -D ARDUINO_LMIC_PROJECT_CONFIG_H_SUPPRESS
    -D CFG_eu868=1
    -D CFG_sx1276_radio=1
```

To map the correct pins of the onboard Lora chip the pinmapping has been changed to:

```
// Pin mapping TTGO LoRa32 V1.0:
const lmic_pinmap lmic_pins = {
    .nss = LORA_CS,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = LORA_RST,
    .dio = {LORA_IRQ, A5, A4},
};
```
