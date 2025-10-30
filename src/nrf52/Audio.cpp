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

void Audio::beginInternal(int sampleRate)
{
    aaAudio.I2S_PIN_LRCK = PIN_AUDIO_LRC;
    aaAudio.I2S_PIN_SCK = PIN_AUDIO_BCK;
    aaAudio.I2S_PIN_SDOUT = PIN_AUDIO_OUT;

    aaAudio.autoAdjust = 0;
    //aaAudio.begin(0 - ADC disabled, 2  - DAC in I2S mode);
    aaAudio.begin(0, 2);
    //aaAudio and/or NRF52 doesn't support 16bit DAC, so we use 8bit only
    aaAudio.dacBitsPerSample = 8;
    aaAudio.setSampleRate(sampleRate);
}

void Audio::playInternal(uint8_t *data, size_t size)
{
    memcpy(aaAudio.dacBuffer, data, size);
    aaAudio.feedDAC(0, size);
}

#endif