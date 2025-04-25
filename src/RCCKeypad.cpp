
#if !defined(ARDUINO_ARCH_NRF52)
#error Architecture/Platform is not supported!
#endif

#include <Arduino.h>


const char *keypadKeys[] = {"loconame",   "locoaddr",     "local" };

const char *keypadValues[] = {"RCC_Keypad", "4", "ON"};

const int keypadKeySize = sizeof(keypadKeys) / sizeof(keypadKeys[0]);
