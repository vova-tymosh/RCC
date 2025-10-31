/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
/*
 * Railroad communication
 *
 *
 */
#if defined(ARDUINO_ARCH_NRF52) || defined(ARDUINO_ARCH_AVR)
#include <stdint.h>
#include <Arduino.h>
#include "nrf52/TransportUtils.h"

void printHex(uint8_t *payload, int size)
{
    for (int i = 0; i < size; i++) {
        Serial.print(payload[i], HEX);
        Serial.print(" ");
    }
}

#endif // #if defined(ARDUINO_ARCH_NRF52)
