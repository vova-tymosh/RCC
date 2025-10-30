/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
#pragma once

#include <stdint.h>
#include <Arduino.h>

class Storage
{
private:
    static const uint16_t code = 0xC0DE;
    static const uint16_t version = 12;

    void deleteFiles();

public:
    void begin(uint16_t _version = version);

    void clear();

    int read(const char *filename, void *buffer, size_t size, size_t offset = 0);

    int write(const char *filename, const void *buffer, size_t size, bool append = false);

    int append(const char *filename, const void *buffer, size_t size);

    bool exists(const char *filename);

    size_t size(const char *filename);

    char *addFolder(char *buffer, const char *folder, const char *filename, size_t size);

    String openFirst();

    String openNext();
};

extern Storage storage;
