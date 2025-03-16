// #if defined(ARDUINO_ARCH_NRF52)


#include <stdint.h>
#include <Arduino.h>
#include <AutoAnalogAudio.h>
#include "Audio.h"
#include "Storage.h"
// #include "Motherboard.h"

#define PIN_AUDIO_LRC 32 + 12 // D7
#define PIN_AUDIO_SCK 32 + 13 // D8
#define PIN_AUDIO_OUT 32 + 14 // D9

AutoAnalog aaAudio;

void Audio::begin() 
{
    // aaAudio.I2S_PIN_LRCK = PIN_AUDIO_LRC; 
    // aaAudio.I2S_PIN_SCK = PIN_AUDIO_SCK;
    // aaAudio.I2S_PIN_SDOUT = PIN_AUDIO_OUT;

    aaAudio.autoAdjust = 0;
    aaAudio.begin(0, 1, 1);
    aaAudio.dacBitsPerSample = 8;
    aaAudio.setSampleRate(16000);
    running = false;
}

void Audio::play(const uint8_t *data, size_t size)
{
    offset = 0;
    this->data = (uint8_t *)data;
    this->size = size;
    running = true;
}

void Audio::play(String filename)
{
    offset = 0;
    playfile = filename;
    running = true;
}

void Audio::loop()
{
    if (!running)
        return;

    int r;
    uint8_t *p;
    if (playfile.length() != 0) {
        r = storage.read(playfile.c_str(), (uint8_t*)buffer, CHUNK_SIZE, offset);
        p = buffer;
        // Serial.print("Play File: ");
        // Serial.println(offset);
    } else {
        p = data + offset;
        r = size - offset;
        if (r > CHUNK_SIZE)
            r = CHUNK_SIZE;
        // Serial.print("Play ROM: ");
        // Serial.println(offset);
    }

    offset += CHUNK_SIZE;
    memcpy(aaAudio.dacBuffer, p, r);
    aaAudio.feedDAC(0, r);

    if (r < CHUNK_SIZE) {
        playfile = "";
        running = false;
        memset(aaAudio.dacBuffer, 0, CHUNK_SIZE);
        aaAudio.feedDAC(0, CHUNK_SIZE);
    }
}

// #endif