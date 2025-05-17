#pragma once
#include "Storage.h"

class Settings
{
    static const int MAX_LENGTH = 256;

    struct {
        char **keys;
        float *values;
        int size;
    } cache;
    bool isCacheEnabled = false;

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
        if (isCacheEnabled) {
            for (int i = 0; i < cache.size; i++) {
                if (strcmp(cache.keys[i], key) == 0)
                    return cache.values[i];
            }
            return 0;
        } else {
            return get(key).toFloat();
        }
    }

    int getCachedInt(const char *key)
    {
        return (int)getCachedFloat(key);
    }

    void put(const char *key, String value)
    {
        if (storage.exists(key)) {
            storage.write(key, (void *)value.c_str(), value.length() + 1);
            for (int i = 0; i < cache.size; i++) {
                if (strcmp(cache.keys[i], key) == 0) {
                    cache.values[i] = value.toFloat();
                    break;
                }
            }
        }
    }

    //Doesn't update cache
    void create(const char *key, const char *value)
    {
        size_t size = strlen(value) + 1;
        storage.write((char *)key, (void *)value, size);
    }
    
    void begin(const char *keys[], const char *values[], const int size, bool _isCacheEnabled = true)
    {
        isCacheEnabled = _isCacheEnabled;
        for (int i = 0; i < size; i++) {
            if (!storage.exists(keys[i])) {
                create(keys[i], values[i]);
            }
        }
        if (isCacheEnabled) {
            cache.size = size;
            cache.keys = (char **)keys;
            cache.values = (float*)malloc(size * sizeof(float));
            for (int i = 0; i < size; i++) {
                if (storage.exists(keys[i])) {
                    String v = get(keys[i]);
                    cache.values[i] = v.toFloat();
                }
            }
        }
    }
};

extern Settings settings;