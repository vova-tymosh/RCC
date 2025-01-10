#pragma once


class Storage {
private:
    const uint16_t code = 0xC0DE;
    const uint16_t version = 9;

    void beginInternal();

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
            setValidation(code << 16 | version);
        }
    }

    void clear();

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

    uint write(const char *filename, void *buffer, size_t size);
};