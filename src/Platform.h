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
