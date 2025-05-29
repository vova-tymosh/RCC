#if defined(ARDUINO_ARCH_ESP32)
#include "../Storage.h"
#include "esp32/Storage.h"

void deleteDirectory(const char *dirname)
{
    char path[256];
    File root = LittleFS.open(dirname);
    File file = root.openNextFile();
    while (file) {
        strcpy(path, dirname);
        if (path[strlen(path) - 1] != '/') {
            strcat(path, "/");
        }
        strcat(path, file.name());
        bool isDir = file.isDirectory();
        file = root.openNextFile();
        if (isDir) {
            deleteDirectory(path);
            LittleFS.rmdir(path);
        } else {
            LittleFS.remove(path);
        }
        // Serial.print("[FS] Delete: ");
        // Serial.println(path);
    }
}

void Storage::deleteFiles()
{
    deleteDirectory("/");
    Serial.println("[FS] All deleted");
}

char* Storage::makeSettingsPath(const char *filename, char *buffer, size_t size)
{
    const char *const prefix = "/settings/";
    strcpy(buffer, prefix);
    strncat(buffer, filename, size - strlen(prefix) - 1);
    return buffer;
}


#endif
