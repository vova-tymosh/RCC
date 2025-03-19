#include <Arduino.h>
#include "Settings.h"

const char *defaultSettings[] = {
    "wifiap", "on",
    "wifissid", "RCC_Loco",
    "wifipwd", "RCC_Loco",
    "loconame", "RCC",
    "locoaddr", "3",
};
const int defaultSettingsSize = sizeof(defaultSettings) / sizeof(defaultSettings[0]);

void Settings::defaults()
{
    for (int i = 0; i < defaultSettingsSize/2; i++) {
        get(defaultSettings[i*2], defaultSettings[i*2 + 1]);
    }
}

Settings settings;