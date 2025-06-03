/*
 * Include only inside Storage.cpp inside platfomr specific folders
 * 
*/
#include "Platform.h"
#include "Storage.h"
#if defined(ARDUINO_ARCH_NRF52) 
#include "nrf52/Storage.h"
#elif defined(ARDUINO_ARCH_ESP32)
#include "esp32/Storage.h"
#elif defined(ARDUINO_ARCH_AVR)
#include "avr/Storage.h"
#endif


const char* const validationFile = "/validation";

void Storage::begin(uint16_t _version)
{
    if (!LittleFS.begin(true)) {
        Serial.println("[FS] Can't mount");
        return;
    }
    uint16_t validation[2];
    read(validationFile, &validation, sizeof(validation));
    if ((validation[0] != code) || (validation[1] != _version)) {
        Serial.println("[FS] Outdated");
        clear();
    }
}

void Storage::clear()
{
    deleteFiles();
    const uint16_t validation[2] = {code, version};
    write(validationFile, (void*)&validation, sizeof(validation));

    char filepath[FILENAME_LEN];
    makeSettingsPath("", filepath, sizeof(filepath));
    if (!LittleFS.mkdir(filepath))
        Serial.println("[FS] Can't create folder");
}

int Storage::read(const char *filename, void *buffer, size_t size,
                  size_t offset)
{
    int r = 0;
    File file = LittleFS.open(filename);
    if (file) {
        file.seek(offset);
        r = file.read((uint8_t *)buffer, size);
        file.close();
    }
    return r;
}

int Storage::write(const char *filename, const void *buffer, size_t size, bool append)
{
    int r = 0;
    File file = LittleFS.open(filename, (append) ? RCC_FILE_APPEND : RCC_FILE_WRITE);
    if (file) {
        if (!append)
            file.seek(0);
        r = file.write((const uint8_t *)buffer, size);
        file.close();
    } else {
        Serial.print("[FS] Can't write: ");
        Serial.println(filename);
    }
    return r;
}

int Storage::append(const char *filename, const void *buffer, size_t size)
{
    return write(filename, buffer, size, true);
}

bool Storage::exists(const char *filename)
{
    return LittleFS.exists(filename);
}

size_t Storage::size(const char *filename)
{
    size_t s = 0;
    File file = LittleFS.open(filename);
    if (file) {
        s = file.size();
        file.close();
    }
    return s;
}


File fileListRoot = BUILD_FILE();

String Storage::openFirst()
{
    char filepath[FILENAME_LEN];
    makeSettingsPath("", filepath, sizeof(filepath));
    fileListRoot = LittleFS.open(filepath);
    return openNext();
}

String Storage::openNext()
{
    if (fileListRoot) {
        File file = fileListRoot.openNextFile();
        if (file)
            return String(file.name());
        else
            fileListRoot.close();
    }
    return String();   
}