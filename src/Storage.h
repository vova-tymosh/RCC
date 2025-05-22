#pragma once
#include <stdint.h>
#include <Arduino.h>


class Storage
{
private:
    const uint16_t code = 0xC0DE;
    const uint16_t version = 11;


    void beginInternal();

    void clearInternal();

public:
    void begin()
    {
        beginInternal();
        uint32_t validation = 0;
        read("validation", &validation, sizeof(validation));
        if (validation >> 16 != code || (validation & 0xFFFF) != version) {
            Serial.println("[FS] No FS or old version, drop to defaults");
            clear();
        }
    }

    void clear()
    {
        clearInternal();
        uint16_t validation[2] = {code, version};
        // (uint32_t)code << 16 | version;
        write("validation", &validation, sizeof(validation));
    }

    int read(const char *filename, void *buffer, size_t size,
             size_t offset = 0);

    int readOrCreate(const char *filename, void *buffer, size_t size)
    {
        int r = read(filename, buffer, size);
        if (size > 0 && r == 0) {
            Serial.print("[FS] File not found, create: ");
            Serial.println(filename);
            write(filename, buffer, size);
        }
        return r;
    }

    int write(const char *filename, void *buffer, size_t size,
              size_t offset = 0);

    bool exists(const char *filename);

    bool allocate(const char *filename, size_t size);

    String openFirst();

    String openNext();
};

extern Storage storage;