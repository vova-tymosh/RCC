#pragma once

#include "RCCNode.h"

#if defined(ARDUINO_ARCH_NRF52) || defined(ARDUINO_AVR_LEONARDO)
#include "nrf52/Transport.h"
#elif defined(ARDUINO_ARCH_ESP32)
#include "esp32/Transport.h"
#else
#error Architecture/Platform is not supported!
#endif
