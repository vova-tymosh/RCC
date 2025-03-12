#pragma once
#include "Storage.h"

class Settings
{
#if defined(HIGH_CAPACITY_STORAGE)
    static const int MAX_LENGTH = 256;
#else
    static const int MAX_LENGTH = 16;
#endif
public:

    void checkDefaults(const char *defaultSettings[], size_t size)
    {
        for (int i = 0; i < size/2; i++) {
            get(defaultSettings[i*2], defaultSettings[i*2 + 1]);
        }
    }

    String get(String key, String defaultValue = "")
    {
        char buffer[MAX_LENGTH];
        int r = storage.read(key.c_str(), buffer, sizeof(buffer));
        if (r == 0) {
            size_t size = defaultValue.length();
            if (size > sizeof(buffer) - 1)
                size = sizeof(buffer) - 1;
            memset(buffer, 0, sizeof(buffer));
            memcpy(buffer, defaultValue.c_str(), size);
            storage.write(key.c_str(), (void*)buffer, sizeof(buffer));
        }
        buffer[MAX_LENGTH - 1] = 0;
        return String(buffer);
    }

    void put(String key, String value)
    {
        storage.write(key.c_str(), (void*)value.c_str(), value.length() + 1);
    }
};

extern Settings settings;