#pragma once

#include "RCCLocoBase.h"

#if defined(ARDUINO_ARCH_NRF52) || defined(ARDUINO_AVR_LEONARDO)
#include "TransportNRF.h"
#elif defined(ARDUINO_ARCH_ESP32) && defined(RCC_NO_STATION)
#include "TransportWT.h"
#define Transport TransportWT
#else
#error Not supported transport
#endif


