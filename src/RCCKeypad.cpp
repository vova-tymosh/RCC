#if defined(ARDUINO_ARCH_NRF52)

#include <Arduino.h>

extern const char *keypadKeys[];
const char *keypadKeys[] = {"loconame", "locoaddr", "test01"};

extern const char *keypadValues[];
const char *keypadValues[] = {"RCC_Keypad", "4", "23"};

extern const int keypadKeySize;
const int keypadKeySize = sizeof(keypadKeys) / sizeof(keypadKeys[0]);

#endif