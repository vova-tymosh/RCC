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
    String get(const char *key)
    {
        char buffer[MAX_LENGTH];
        buffer[0] = 0;
        storage.read(key, buffer, sizeof(buffer));
        return String(buffer);
    }

    void put(const char *key, String value)
    {
        if (storage.exists(key))
            storage.write(key, (void *)value.c_str(), value.length() + 1);
    }

    void create(const char *key, const char *value)
    {
        char buffer[MAX_LENGTH];
        size_t size = strlen(value);
        if (size > sizeof(buffer) - 1)
            size = sizeof(buffer) - 1;
        memset(buffer, 0, sizeof(buffer));
        memcpy(buffer, value, size);
        storage.write((char *)key, (void *)buffer, sizeof(buffer));
    }

    void defaults(const char *keys[], const char *values[], const int size)
    {
        for (int i = 0; i < size; i++) {
            if (!storage.exists(keys[i]))
                create(keys[i], values[i]);
        }
    }
};

extern Settings settings;