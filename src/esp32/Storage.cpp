/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
#if defined(ARDUINO_ARCH_ESP32)
#include "../Storage.h"
#include "esp32/Storage.h"

void listDirectory(const char *dirname)
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
            Serial.print("d ");
            Serial.println(path);
            listDirectory(path);
        } else {
            Serial.print("f ");
            Serial.println(path);
        }
    }
}

void Storage::list()
{
    listDirectory("/");
}

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

#endif
