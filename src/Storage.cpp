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

#define MAKE_PATH(name) \
    char path[256]; \
    path[0] = '/'; \
    strcpy(path + 1, name); \

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
    MAKE_PATH(filename);
    File file = fs.open(path);
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
    MAKE_PATH(filename); 
    File file = fs.open(path, F_WRITE_MODE);
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
    MAKE_PATH(filename);
    return fs.exists(path);
}

bool Storage::allocate(const char *filename, size_t size)
{
    return true;
}

File fileListRoot = BUILD_FILE();

String Storage::openFirst()
{
    fileListRoot = fs.open("/");
    return openNext();
}

String Storage::openNext()
{
    if (fileListRoot) {
        File file = fileListRoot.openNextFile();
        while (file) {
            String s = String(file.name());
            if (s == "validation") {
                file = fileListRoot.openNextFile();
                continue;
            } else {
                return s;
            }
        }
        fileListRoot.close();
    }
    return String();   
}