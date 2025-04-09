#if defined(ARDUINO_ARCH_ESP32)

#include <stdint.h>
#include <Arduino.h>
#include <ESP_I2S.h>
#include "Audio.h"
#include "Storage.h"
#include "Motherboard.h"

i2s_data_bit_width_t bps = I2S_DATA_BIT_WIDTH_16BIT;
i2s_mode_t mode = I2S_MODE_STD;
i2s_slot_mode_t slot = I2S_SLOT_MODE_MONO;
// i2s_std_slot_mask_t slotMask = I2S_STD_SLOT_BOTH;

const int sampleRate = 8000;

I2SClass i2s;

void Audio::beginInternal()
{
    i2s.setPins(PIN_AUDIO_BCK, PIN_AUDIO_LRC, PIN_AUDIO_OUT);

    if (!i2s.begin(mode, sampleRate, bps, slot)) {
        Serial.println("[AUD] Failed to initialize I2S");
        while (1)
            ;
    }

    running = false;
}

void Audio::playInternal(uint8_t *data, size_t size, int volumeDivider)
{
    if (volumeDivider != 1) {
        for (int i = 0; i < size; i++) {
            data[i] /= volumeDivider;
        }
    }
    i2s.write((const uint8_t *)data, size);
}

#endif