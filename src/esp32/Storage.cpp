#if defined(ARDUINO_ARCH_ESP32)

#include <FS.h>
#include <LittleFS.h>
#include "Storage.h"

#define fs LittleFS
#define F_WRITE_MODE ((offset) ? FILE_APPEND : FILE_WRITE)
#define getPath(x) x.path()

void Storage::beginInternal()
{
    if (!fs.begin(true)) {
        Serial.println("[FS] Mount failed");
        return;
    }
    Serial.println("[FS] Mount successful");
}

#include "StorageImpl.h"
#endif
