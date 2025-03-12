#if defined(ARDUINO_ARCH_ESP32)

#include "FS.h"
#include "SPIFFS.h"
#include "Storage.h"

void Storage::beginInternal()
{
    if (!SPIFFS.begin(true)) {
        Serial.println("[FS] Mount failed");
        return;
    }
    Serial.println("[FS] Mount successful");
}

uint32_t Storage::getValidation()
{
    uint32_t validation = 0;
    read("validation", &validation, sizeof(validation));
    return validation;
}

void Storage::setValidation(uint32_t validation)
{
    write("validation", &validation, sizeof(validation));
}

void Storage::clearInternal()
{
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    while (file) {
        Serial.println("[FS] Delete: " + String(file.path()));
        SPIFFS.remove(file.path());
        file = root.openNextFile();
    }
}

int Storage::read(const char *filename, void *buffer, size_t size, size_t offset)
{
    int r = 0;
    File file = SPIFFS.open(String("/") + filename);
    if (file) {
        if (offset >= file.size())
            return 0;
        if (offset + size > file.size())
            size -= offset + size - file.size();
        file.seek(offset);
        r = file.read((uint8_t*)buffer, size);
        file.close();
    }
    return r;
}

int Storage::write(const char *filename, void *buffer, size_t size, size_t offset)
{
    int r = 0;
    File file = SPIFFS.open(String("/") + filename, FILE_WRITE);
    if (file) {
        file.seek(offset);
        r = file.write((const uint8_t*)buffer, size);
        file.close();
    } else {
        Serial.print("[FS] Failed to write file: ");
        Serial.println(filename);
    }
    return r;
}

bool Storage::allocate(const char *filename, size_t size)
{
    return true;
}

#endif
