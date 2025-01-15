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

    void put(String key, String value)
    {
        storage.write(key.c_str(), (void*)value.c_str(), value.length() + 1);
    }
};

extern Settings settings;