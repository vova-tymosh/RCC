#pragma once
#include "Storage.h"
#include "Platform.h"

class Settings
{
    struct {
        char **keys;
        float *values;
        int size;
    } cache;

public:
    String get(const char *key)
    {
        char buffer[VALUE_LEN];
        get(key, buffer, sizeof(buffer));
        return String(buffer);
    }

    void get(const char *key, char *value, size_t size)
    {
        char filepath[FILENAME_LEN];
        value[0] = 0;
        storage.makeSettingsPath(key, filepath, sizeof(filepath));
        storage.read(filepath, value, size);
    }

    float getCachedFloat(const char *key)
    {
        for (int i = 0; i < cache.size; i++) {
            if (strcmp(cache.keys[i], key) == 0)
                return cache.values[i];
        }
        return 0;
    }

    int getCachedInt(const char *key)
    {
        return (int)getCachedFloat(key);
    }

    void put(const char *key, String value)
    {
        put(key, value.c_str());
    }

    void put(const char *key, const char *value)
    {
        char filepath[FILENAME_LEN];
        storage.makeSettingsPath(key, filepath, sizeof(filepath));
        if (storage.exists(filepath)) {
            storage.write(filepath, (void *)value, strlen(value) + 1);
            for (int i = 0; i < cache.size; i++) {
                if (strcmp(cache.keys[i], key) == 0) {
                    cache.values[i] = atof(value);
                    break;
                }
            }
        }
    }

    // Doesn't update cache
    void create(const char *key, const char *value)
    {
        char filepath[FILENAME_LEN];
        storage.makeSettingsPath(key, filepath, sizeof(filepath));
        size_t size = strlen(value) + 1;
        storage.write(filepath, (void *)value, size);
    }

    void begin(const char *keys[], const char *values[], const int size)
    {
        for (int i = 0; i < size; i++) {
            char filepath[FILENAME_LEN];
            storage.makeSettingsPath(keys[i], filepath, sizeof(filepath));
            if (!storage.exists(filepath)) {
                size_t size = strlen(values[i]) + 1;
                storage.write(filepath, (void *)values[i], size);
            }
        }
        cache.size = size;
        cache.keys = (char **)keys;
        cache.values = (float *)malloc(size * sizeof(float));

        for (int i = 0; i < size; i++) {
            char value[VALUE_LEN];
            get(keys[i], value, sizeof(value));
            cache.values[i] = atof(value);
        }
    }
};

extern Settings settings;