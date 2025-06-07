/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <Arduino.h>
#include "Storage.h"

// TODO: mix multiple audios

class Audio
{
    static const int CHUNK_SIZE = 512;
    uint8_t buffer[CHUNK_SIZE];
    String playfile;
    int offset;
    uint8_t *data;
    size_t size;
    int volumeDivider;
    bool running;

    void beginInternal();

    void playInternal(uint8_t *data, size_t size, int volumeDivider = 1);

public:
    bool cycle;

    void begin()
    {
        beginInternal();
        running = false;
        cycle = false;
    }

    void play(const uint8_t *data, size_t size, int volumeDivider = 1)
    {
        offset = 0;
        this->data = (uint8_t *)data;
        this->size = size;
        this->volumeDivider = volumeDivider;
        running = true;
    }

    void play(String filename, int volumeDivider = 1)
    {
        offset = 0;
        playfile = filename;
        this->volumeDivider = volumeDivider;
        running = true;
    }

    void loop()
    {
        if (!running)
            return;

        int r;
        if (playfile.length() != 0) {
            r = storage.read(playfile.c_str(), (uint8_t *)buffer, CHUNK_SIZE,
                             offset);
        } else {
            r = size - offset;
            if (r > CHUNK_SIZE)
                r = CHUNK_SIZE;
            memcpy(buffer, data + offset, r);
        }

        offset += CHUNK_SIZE;
        playInternal(buffer, r, this->volumeDivider);

        if (r < CHUNK_SIZE) {
            offset = 0;
            memset(buffer, 0, CHUNK_SIZE);
            playInternal(buffer, CHUNK_SIZE);
            if (!cycle) {
                playfile = "";
                running = false;
            }
        }
    }
};