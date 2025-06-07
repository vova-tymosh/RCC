/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
#pragma once

#if defined(ARDUINO_ARCH_NRF52)

#define FILENAME_LEN 256
#define VALUE_LEN 256

#elif defined(ARDUINO_ARCH_ESP32)

#define FILENAME_LEN 256
#define VALUE_LEN 256

#else

#warning Assuming low memory platform
#define FILENAME_LEN 12
#define VALUE_LEN 32

#endif
