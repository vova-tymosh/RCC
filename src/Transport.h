#pragma once

#include "RCCLocoBase.h"

#if defined(ARDUINO_ARCH_NRF52) || defined(ARDUINO_AVR_LEONARDO)
#include "TransportNRF.h"
#define Transport TransportNRF
#elif defined(ARDUINO_ARCH_ESP32)
#if defined(RCC_NO_STATION)
#include "TransportWT.h"
#define Transport TransportWT
#else
#error MQTT is not implemented yet
#endif
#else
#error Architecture/Platform is not supported!
#endif
