#pragma once
#include "Storage.h"

class Settings
{
    Storage &storage;
public:
    Settings(Storage &storage) : storage(storage) {}

    String get(String key)
    {
        char buffer[256];
        storage.readOrCreate(key.c_str(), buffer, sizeof(buffer));
        return String(buffer);
    }

    String get(String key, String defaultValue)
    {
        char buffer[256];
        uint r = storage.read(key.c_str(), buffer, sizeof(buffer));
        if (r == 0) {
            storage.write(key.c_str(), (void*)defaultValue.c_str(), defaultValue.length() + 1);
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