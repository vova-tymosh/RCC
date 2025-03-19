#if defined(ARDUINO_ARCH_NRF52)


#include <AutoAnalogAudio.h>
#include "Audio.h"

// #include "Motherboard.h"

#define PIN_AUDIO_LRC 32 + 12 // D7
#define PIN_AUDIO_SCK 32 + 13 // D8
#define PIN_AUDIO_OUT 32 + 14 // D9

// #define PIN_AUDIO_LRC 2 //32 + 12 // D7
// #define PIN_AUDIO_SCK 3 //32 + 13 // D8
// #define PIN_AUDIO_OUT 28 //32 + 14 // D9


AutoAnalog aaAudio;

void Audio::beginInternal() 
{
    aaAudio.I2S_PIN_LRCK = PIN_AUDIO_LRC; 
    aaAudio.I2S_PIN_SCK = PIN_AUDIO_SCK;
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