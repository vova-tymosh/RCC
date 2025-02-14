#pragma once

#define PIN_NOTHING   D0
#define PIN_MOTOR_EMF D1
#define PIN_MOTOR_BCK D2
#define PIN_MOTOR_FWD D3
#define PIN_SPEED_PIN D6
#define PIN_AUDIO_LRC 32 + 12 // D7
#define PIN_AUDIO_SCK 32 + 13 // D8
#define PIN_AUDIO_OUT 32 + 14 // D9


const char *defaultSettings[] = {
        "wifiap", "on",
        "wifissid", "RCC_Loco",
        "wifipwd", "RCC_Loco",
        "loconame", "RCC",
        "locoaddr", "3",
};
const int defaultSettingsSize = sizeof(defaultSettings) / sizeof(defaultSettings[0]);