// #pragma once
#ifndef STORAGE_H
#define STORAGE_H


#include <stdint.h>
#include <Arduino.h>


class Storage
{
private:
    static const uint16_t code = 0xC0DE;
    static const uint16_t version = 12;
    static const int MAX_LENGTH = 256;

    void deleteFiles();

public:
    void begin(uint16_t _version = version);

    void clear();

    int read(const char *filename, void *buffer, size_t size,
             size_t offset = 0);

    int write(const char *filename, const void *buffer, size_t size,
              size_t offset = 0);

    bool exists(const char *filename);

    char* makeSettingsPath(const char *filename, char *buffer, size_t size);

    String openFirst();

    String openNext();
};

extern Storage storage;

#endif // STORAGE_H