#pragma once
#ifdef ARDUINO_ARCH_NRF52
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
#else
#include <EEPROM.h>
#endif


#ifdef ARDUINO_ARCH_NRF52

class Storage {
    const int SIZE = 256;
    const char *FILENAME = "/rcc.cfg";
    Adafruit_LittleFS_Namespace::File file;
    const int FILE_O_READ = Adafruit_LittleFS_Namespace::FILE_O_READ;
    const int FILE_O_WRITE = Adafruit_LittleFS_Namespace::FILE_O_WRITE;

  public:
    Storage(): file(InternalFS) {}

    void setup() {
      InternalFS.begin();
      file.open(FILENAME, FILE_O_READ);
      if (!file) {
        uint32_t data = 0;
        file.open(FILENAME, FILE_O_WRITE);
        for (int i = 0; i < SIZE / sizeof(data); i++)
          file.write((uint8_t*)&data, sizeof(data));
        file.close();
      }
    }

    uint16_t restore(uint16_t offset = 0) {
      file.open(FILENAME, FILE_O_READ);
      file.seek(offset);
      uint16_t data;
      if (file) {
        file.read(&data, sizeof(data));
        file.close();
      }
      return data;
    }

    void save(uint16_t data, uint16_t offset = 0) {
      file.open(FILENAME, FILE_O_WRITE);
      file.seek(offset);
      file.write((uint8_t*)&data, sizeof(data));
      file.close();
    }
};

#else

class Storage {
  const int SIZE = 256;
  const int ADDR_VALIDATION = 0;
  const int ADDR_DATA = 1;

  public:
    void setup() {
      uint8_t validation = 0;
      EEPROM.get(ADDR_VALIDATION, validation);
      if (validation == 0xFF) {
        for (int i = 0; i < SIZE; i++) {
          EEPROM.write(i, 0);
        }
        Serial.println(F("First time EEPROM init"));
      }
    }

    uint16_t restore(uint16_t offset = 0) {
      if (offset < SIZE) {
        uint16_t data = 0;
        setup();
        EEPROM.get(ADDR_DATA + offset, data);
        return data;
      }
    }

    void save(uint16_t data, uint16_t offset = 0) {
      if (offset < SIZE) {
        EEPROM.put(ADDR_DATA + offset, data);
      }
    }
};

#endif

extern Storage storage;
