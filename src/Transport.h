#pragma once

#include "RCCLocoBase.h"

#if defined(ARDUINO_ARCH_NRF52) || defined(ARDUINO_AVR_LEONARDO)
#include "nrf52/Transport.h"
#elif defined(ARDUINO_ARCH_ESP32)
#if defined(RCC_NO_STATION)
#include "esp32/Transport.h"
#else
#error MQTT is not implemented yet
#endif
#else
#error Architecture/Platform is not supported!
#endif
