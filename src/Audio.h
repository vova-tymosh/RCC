#pragma once

#include <stddef.h>
#include <stdint.h>


class Audio
{
    static const int CHUNK_SIZE = 512;
    uint8_t buffer[CHUNK_SIZE];
    String playfile;
    int offset;
    uint8_t *data;
    size_t size;
    bool running;

public:
    
    void begin();

    void play(const uint8_t *data, size_t size);

    void play(String filename);

    void loop();
};