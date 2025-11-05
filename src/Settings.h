/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
#pragma once
#include "Storage.h"
#include "Platform.h"

struct KeyValue {
    const char *key;
    const char *value;
};


class Settings
{
    struct {
        char *keyBuffer;      // Single buffer for all key strings
        char **keys;          // Pointers into keyBuffer
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
        storage.addFolder(filepath, SETTINGS_PATH, key, sizeof(filepath));
        int r = storage.read(filepath, value, size);
        value[r] = '\0';
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

    void set(const char *key, String value)
    {
        set(key, value.c_str());
    }

    void set(const char *key, const char *value)
    {
        char filepath[FILENAME_LEN];
        storage.addFolder(filepath, SETTINGS_PATH, key, sizeof(filepath));
        if (storage.exists(filepath)) {
            storage.write(filepath, (void *)value, strlen(value));
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
        storage.addFolder(filepath, SETTINGS_PATH, key, sizeof(filepath));
        size_t size = strlen(value);
        storage.write(filepath, (void *)value, size);
    }

    void begin(const KeyValue defaults[], const int size)
    {
        for (int i = 0; i < size; i++) {
            char filepath[FILENAME_LEN];
            storage.addFolder(filepath, SETTINGS_PATH, defaults[i].key, sizeof(filepath));
            if (!storage.exists(filepath)) {
                size_t valueSize = strlen(defaults[i].value);
                storage.write(filepath, (void *)defaults[i].value, valueSize);
            }
        }
        buildCache();
    }

    void begin()
    {
        buildCache();
    }

private:
    void buildCache()
    {
        // Count files and calculate total key length
        cache.size = 0;
        size_t totalKeyLength = 0;
        String name = storage.openFirst(SETTINGS_PATH);
        while (name.length() > 0) {
            cache.size++;
            totalKeyLength += name.length() + 1;  // +1 for null terminator
            name = storage.openNext();
        }

        if (cache.size == 0)
            return;

        // Three allocations: key buffer, key pointers, values
        cache.keyBuffer = (char *)malloc(totalKeyLength);
        cache.keys = (char **)malloc(cache.size * sizeof(char *));
        cache.values = (float *)malloc(cache.size * sizeof(float));

        // Populate cache
        int i = 0;
        size_t offset = 0;
        name = storage.openFirst(SETTINGS_PATH);
        while (name.length() > 0) {
            // Point to position in buffer
            cache.keys[i] = cache.keyBuffer + offset;

            // Copy key string
            strcpy(cache.keys[i], name.c_str());
            offset += name.length() + 1;

            // Read and cache value
            char value[VALUE_LEN];
            get(cache.keys[i], value, sizeof(value));
            cache.values[i] = atof(value);

            i++;
            name = storage.openNext();
        }
    }

public:
};

extern Settings settings;