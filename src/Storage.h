#pragma once
#ifdef ARDUINO_ARCH_NRF52
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
#else
#include <EEPROM.h>
#endif

#ifdef ARDUINO_ARCH_NRF52

class Storage
{
    const int SIZE = 256;
    const char *FILENAME = "/rcc.cfg";
    Adafruit_LittleFS_Namespace::File file;
    const int FILE_O_READ = Adafruit_LittleFS_Namespace::FILE_O_READ;
    const int FILE_O_WRITE = Adafruit_LittleFS_Namespace::FILE_O_WRITE;

public:
    Storage() : file(InternalFS) {}

    void setup(const char* version = NULL)
    {
        InternalFS.begin();
        file.open(FILENAME, FILE_O_READ);
        if (!file) {
            uint32_t data = 0;
            file.open(FILENAME, FILE_O_WRITE);
            for (int i = 0; i < SIZE / sizeof(data); i++)
                file.write((uint8_t *)&data, sizeof(data));
            file.close();
            Serial.println("First time FLASH init");
        }
    }

    uint32_t restore(uint8_t offset = 0)
    {
        uint32_t data;
        file.open(FILENAME, FILE_O_READ);
        file.seek(offset * sizeof(data));
        if (file) {
            file.read(&data, sizeof(data));
            file.close();
        }
        return data;
    }

    void save(uint32_t data, uint8_t offset = 0)
    {
        file.open(FILENAME, FILE_O_WRITE);
        file.seek(offset * sizeof(data));
        file.write((uint8_t *)&data, sizeof(data));
        file.close();
    }
};

#else

class Storage
{
    const int SIZE = 256;
    const int OFFSET_VERSION = 0;
    const int OFFSET_DATA = 10;

    void clear()
    {
        for (int i = 0; i < SIZE; i++) {
            EEPROM.write(i, 0);
        }
    }

public:
    void setup(const char* version = NULL)
    {
        Serial.println(F("EEPROM setup"));
        uint8_t validation = EEPROM.read(0);
        if (validation == 0xFF) {
            clear();
            Serial.println(F("First time EEPROM init"));
        } else if (version != NULL) {
            char v[OFFSET_DATA - OFFSET_VERSION];
            for (int i = 0; i < sizeof(v); i++) {
                v[i] = EEPROM.read(OFFSET_VERSION + i);
            }
            if (strncmp(v, version, sizeof(v)) != 0) {
                clear();
                for (int i = 0; i < sizeof(v); i++) {
                    EEPROM.write(OFFSET_VERSION + i, version[i]);
                }
                Serial.print(F("EEPROM version mismatch, old: "));
                Serial.print(v);
                Serial.print(F(", "));
                Serial.print(strncmp(v, version, sizeof(v)));
                Serial.print(F(", new: "));
                Serial.println(version);
            }
        }
    }

    uint32_t restore(uint8_t offset = 0)
    {
        if (offset < SIZE) {
            uint32_t data = 0;
            EEPROM.get(OFFSET_DATA + offset * sizeof(data), data);
            return data;
        }
    }

    void save(uint32_t data, uint8_t offset = 0)
    {
        if (offset < SIZE) {
            EEPROM.put(OFFSET_DATA + offset * sizeof(data), data);
        }
    }
};

#endif

extern Storage storage;
