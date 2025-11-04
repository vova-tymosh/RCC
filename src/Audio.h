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

class Audio
{
public:
    static const int MAX_CHANNELS = 4;

private:
    static const int CHUNK_SIZE = 2048;

    struct Channel {
        String filename;
        int offset;
        bool active;
        bool cycle;
        uint8_t volume; // 0-255, where 255 is full volume
        uint8_t buffer[CHUNK_SIZE];
    };

    Channel channels[MAX_CHANNELS];
    int16_t mixBuffer[CHUNK_SIZE / 2]; // For 16-bit PCM mixing
    // uint8_t outputBuffer[CHUNK_SIZE];
    bool running;

    void beginInternal(int sampleRate);
    void playInternal(uint8_t *data, size_t size);

    int readChannelData(Channel &channel, uint8_t *buffer, int maxSize)
    {
        if (!channel.active)
            return 0;

        int r = storage.read(channel.filename.c_str(), buffer, maxSize, channel.offset);
        channel.offset += r;

        if (r < maxSize) {
            if (channel.cycle) {
                channel.offset = 0;
            } else {
                channel.active = false;
            }
        }
        return r;
    }

    void mixChannels()
    {
        int samplesRead = 0;
        memset(mixBuffer, 0, sizeof(mixBuffer));

        // Read and mix all active channels
        for (int ch = 0; ch < MAX_CHANNELS; ch++) {
            if (!channels[ch].active)
                continue;

            int r = readChannelData(channels[ch], channels[ch].buffer, CHUNK_SIZE);
            if (r == 0)
                continue;

            if (samplesRead == 0)
                samplesRead = r;

            // Mix 16-bit PCM samples with volume
            int16_t *samples = (int16_t *)channels[ch].buffer;
            int numSamples = r / 2;
            uint8_t vol = channels[ch].volume;

            for (int i = 0; i < numSamples; i++) {
                // Apply volume (multiply by volume/255)
                int32_t sample = (samples[i] * vol) >> 8;
                int32_t mixed = mixBuffer[i] + sample;

                // Clamp to prevent overflow
                if (mixed > 32767)
                    mixed = 32767;
                else if (mixed < -32768)
                    mixed = -32768;
                mixBuffer[i] = (int16_t)mixed;
            }
        }

        // Convert back to bytes
        if (samplesRead > 0) {
            playInternal((uint8_t *)mixBuffer, samplesRead);
        }
    }

public:
    void begin(int sampleRate)
    {
        beginInternal(sampleRate);
        running = false;
        for (int i = 0; i < MAX_CHANNELS; i++) {
            channels[i].active = false;
            channels[i].cycle = false;
            channels[i].offset = 0;
            channels[i].volume = 255; // Default to full volume
        }
    }

    int play(String filename, uint8_t volume = 255, bool cycle = false)
    {
        // Find available channel
        for (int i = 0; i < MAX_CHANNELS; i++) {
            if (!channels[i].active) {
                channels[i].filename = filename;
                channels[i].offset = 0;
                channels[i].active = true;
                channels[i].cycle = cycle;
                channels[i].volume = volume;
                running = true;
                return i;
            }
        }
        return -1; // No available channel
    }

    void setVolume(int channel, uint8_t volume)
    {
        if (channel >= 0 && channel < MAX_CHANNELS) {
            channels[channel].volume = volume;
        }
    }

    void stop(int channel)
    {
        if (channel >= 0 && channel < MAX_CHANNELS) {
            channels[channel].active = false;
        }
    }

    void stopAll()
    {
        for (int i = 0; i < MAX_CHANNELS; i++) {
            channels[i].active = false;
        }
        running = false;
    }

    bool isPlaying(int channel)
    {
        if (channel >= 0 && channel < MAX_CHANNELS) {
            return channels[channel].active;
        }
        return false;
    }

    void loop()
    {
        if (!running)
            return;

        mixChannels();

        // Check if any channels are still active
        bool anyActive = false;
        for (int i = 0; i < MAX_CHANNELS; i++) {
            if (channels[i].active) {
                anyActive = true;
                break;
            }
        }
        running = anyActive;
    }
};