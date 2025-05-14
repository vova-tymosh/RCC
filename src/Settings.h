#pragma once
#include "Storage.h"

class Settings
{
#if defined(HIGH_CAPACITY_STORAGE)
    static const int MAX_LENGTH = 256;
#else
    static const int MAX_LENGTH = 16;
#endif

    struct {
        char **keys;
        float *values;
        int size;
    } cache;


public:
    String get(const char *key)
    {
        char buffer[MAX_LENGTH];
        buffer[0] = 0;
        storage.read(key, buffer, sizeof(buffer));
        return String(buffer);
    }

    float getCachedFloat(const char *key)
    {
        for (int i = 0; i < cache.size; i++) {
            if (strcmp(cache.keys[i], key) == 0)
                return cache.values[i];
        }
        return 0;
    }

    void put(const char *key, String value)
    {
        if (storage.exists(key))
            storage.write(key, (void *)value.c_str(), value.length() + 1);
    }

    //Doesn't update cache
    void create(const char *key, const char *value)
    {
        // char buffer[MAX_LENGTH];
        size_t size = strlen(value) + 1;
        // if (size > sizeof(buffer) - 1)
        //     size = sizeof(buffer) - 1;
        // memset(buffer, 0, sizeof(buffer));
        // memcpy(buffer, value, size);
        storage.write((char *)key, (void *)value, size);
    }
    
    void defaults(const char *keys[], const char *values[], const int size)
    {
        cache.size = size;
        cache.keys = (char **)keys;
        cache.values = (float*)malloc(size * sizeof(float));
        for (int i = 0; i < size; i++) {
            if (storage.exists(keys[i])) {
                String v = get(keys[i]);
                cache.values[i] = v.toFloat();
            } else {
                cache.values[i] = atof(values[i]);
                create(keys[i], values[i]);
            }
        }
    }
};

extern Settings settings;