/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
#pragma once

#include "RCCNode.h"

#if defined(ARDUINO_ARCH_NRF52) || defined(ARDUINO_ARCH_AVR)
#include "nrf52/Transport.h"
#elif defined(ARDUINO_ARCH_ESP32)
#include "esp32/Transport.h"
#else
#error Architecture/Platform is not supported!
#endif
