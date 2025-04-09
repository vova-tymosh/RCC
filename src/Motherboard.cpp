#include "Motherboard.h"

const char *settingsKeys[] = {"wifiap",   "wifissid", "wifipwd",   "loconame",
                              "locoaddr", "broker",   "brokerport"};

const char *settingsValues[] = {"on", "RCC_Loco",      "RCC_Loco", "RCC",
                                "3",  "192.168.20.61", "1883"};

const int settingsSize = sizeof(settingsKeys) / sizeof(settingsKeys[0]);
// const int settingsValueSize = sizeof(settingsValues) /
// sizeof(settingsValues[0]);
