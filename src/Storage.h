#pragma once


class Storage {
private:
    const uint16_t code = 0xC0DE;
    const uint16_t version = 9;

    void beginInternal();

    void clearInternal();

    uint32_t getValidation();

    void setValidation(uint32_t value);

public:

    void begin()
    {
        beginInternal();
        uint32_t validation = getValidation();
        if (validation >> 16 != code || (validation & 0xFFFF) != version) {
            Serial.println("[FS] No FS or old version, reformat");
            clear();
        }
    }

    void clear()
    {
        clearInternal();
        setValidation(code << 16 | version);
    }

    uint read(const char *filename, void *buffer, size_t size, uint offset = 0);

    uint readOrCreate(const char *filename, void *buffer, size_t size)
    {
        uint r = read(filename, buffer, size);
        if (size > 0 && r == 0) {
            Serial.print("[FS] File not found, create: ");
            Serial.println(filename);
            write(filename, buffer, size);
        }
        return r;
    }

    uint write(const char *filename, void *buffer, size_t size, uint offset = 0);

    bool allocate(const char *filename, size_t size);
};

extern Storage storage;