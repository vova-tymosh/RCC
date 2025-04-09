#pragma once

#if defined(ARDUINO_ARCH_NRF52)
#include "Adafruit_SPIFlash.h"

const SPIFlash_Device_t XIAO_NRF_FLASH = P25Q16H;
Adafruit_FlashTransport_QSPI flashTransport;
Adafruit_SPIFlash flash(&flashTransport);
const uint16_t maxFiles = 64;

#define phyBegin() flash.begin(&XIAO_NRF_FLASH, 1);
#define phyErase()                                                             \
    {                                                                          \
        flash.eraseChip();                                                     \
        flash.waitUntilReady();                                                \
    };
#define phyRead(offset, data, size) flash.readBuffer(offset, data, size);
#define phyWrite(offset, data, size) flash.writeBuffer(offset, data, size);
#define phySize (2 * 1024 * 1024)

#else
#include "EEPROM.h"

const uint16_t maxFiles = 8;

int eeprom_read(int offset, uint8_t *data, size_t size)
{
    for (int i = 0; i < size; i++) {
        data[i] = EEPROM.read(offset + i);
    }
    return size;
}
int eeprom_write(int offset, uint8_t *data, size_t size)
{
    for (int i = 0; i < size; i++) {
        EEPROM.write(offset + i, data[i]);
    }
    return size;
}

#define phyBegin() ;
#define phyErase()                                                             \
    {                                                                          \
        for (int i = 0; i < EEPROM.length(); i++)                              \
            EEPROM.write(i, 0);                                                \
    };
#define phyRead(offset, data, size) eeprom_read(offset, data, size);
#define phyWrite(offset, data, size) eeprom_write(offset, data, size);
#define phySize (EEPROM.length())

#endif
