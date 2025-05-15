/*
 * Include only inside Storage.cpp inside platfomr specific folders
 * 
*/
#include "Storage.h"
#if defined(ARDUINO_ARCH_NRF52) 
#include "nrf52/Storage.h"
#elif defined(ARDUINO_ARCH_ESP32)
#include "esp32/Storage.h"
#elif defined(ARDUINO_AVR_LEONARDO)
#include "avr/Storage.h"
#endif

void Storage::beginInternal()
{
    beginPhy();
    if (!fs.begin()) {
        bool r = fs.format();
        r = r && fs.begin();
        if (!r)
            Serial.println("[FS] Format failed");
    } else {
        Serial.println("[FS] Mount successful");
    }
}

void Storage::clearInternal()
{
    File root = fs.open("/");
    File file = root.openNextFile();
    while (file) {
        String path = String("/") + file.name();
        Serial.println("[FS] Delete: " + path);
        fs.remove(path.c_str());
        file = root.openNextFile();
    }
}

int Storage::read(const char *filename, void *buffer, size_t size,
                  size_t offset)
{
    int r = 0;
    String path = String("/") + filename;
    File file = fs.open(path.c_str());
    if (file) {
        file.seek(offset);
        r = file.read((uint8_t *)buffer, size);
        file.close();
    }
    return r;
}

int Storage::write(const char *filename, void *buffer, size_t size,
                   size_t offset)
{
    int r = 0;
    String path = String("/") + filename;
    File file = fs.open(path.c_str(), F_WRITE_MODE);
    if (file) {
        file.seek(offset);
        r = file.write((const uint8_t *)buffer, size);
        file.close();
    } else {
        Serial.print("[FS] Failed to write file: ");
        Serial.println(filename);
    }
    return r;
}

bool Storage::exists(const char *filename)
{
    String path = String("/") + filename;
    return fs.exists(path.c_str());
}

bool Storage::allocate(const char *filename, size_t size)
{
    return true;
}
