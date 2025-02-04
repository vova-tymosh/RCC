#pragma once
#include "Storage.h"

class Settings
{
    const int MAX_LENGTH = 256;
    Storage &storage;
public:
    Settings(Storage &storage) : storage(storage) {}

    void checkDefaults(const char *defaultSettings[], size_t size)
    {
        for (int i = 0; i < size/2; i++) {
            get(defaultSettings[i*2], defaultSettings[i*2 + 1]);
        }
    }

    String get(String key)
    {
        char buffer[MAX_LENGTH];
        storage.readOrCreate(key.c_str(), buffer, sizeof(buffer));
        return String(buffer);
    }

    String get(String key, String defaultValue)
    {
        char buffer[MAX_LENGTH];
        uint r = storage.read(key.c_str(), buffer, sizeof(buffer));
        if (r == 0) {
            memset(buffer, 0, sizeof(buffer));
            memcpy(buffer, defaultValue.c_str(), defaultValue.length());
            storage.write(key.c_str(), (void*)buffer, MAX_LENGTH);
            return defaultValue;
        }
        return String(buffer);
    }

    void put(String key, String value)
    {
        storage.write(key.c_str(), (void*)value.c_str(), value.length() + 1);
    }
};

extern Settings settings;