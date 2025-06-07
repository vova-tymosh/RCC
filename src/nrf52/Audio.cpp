/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
#if defined(ARDUINO_ARCH_NRF52)

#include <AutoAnalogAudio.h>
#include "Audio.h"
#include "Motherboard.h"

AutoAnalog aaAudio;

void Audio::beginInternal()
{
    aaAudio.I2S_PIN_LRCK = PIN_AUDIO_LRC;
    aaAudio.I2S_PIN_SCK = PIN_AUDIO_BCK;
    aaAudio.I2S_PIN_SDOUT = PIN_AUDIO_OUT;

    aaAudio.autoAdjust = 0;
    aaAudio.begin(0, 1, 1);
    aaAudio.dacBitsPerSample = 8;
    aaAudio.setSampleRate(16000);
}

void Audio::playInternal(uint8_t *data, size_t size, int volumeDivider)
{
    if (volumeDivider != 1) {
        for (int i = 0; i < size; i++) {
            aaAudio.dacBuffer[i] = data[i] / volumeDivider;
        }
    } else {
        memcpy(aaAudio.dacBuffer, data, size);
    }
    aaAudio.feedDAC(0, size);
}

#endif