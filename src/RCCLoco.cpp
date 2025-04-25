/*
 * Railroad communication
 *
 *
 */
#include <Arduino.h>

extern const char *locoKeys[];
const char *locoKeys[] = {"wifiap",     "wifissid",     "wifipwd",
    "loconame",   "locoaddr",     "broker",  "brokerport", 
    "acceleration", "managespeed", "heartbeat"};

extern const char *locoValues[];
const char *locoValues[] = {"on",   "RCC_Loco", "RCC_Loco",
      "RCC",  "3",        "192.168.20.61",  "1883", 
      "0",    "0",        "1000"};

extern const int locoKeySize;
const int locoKeySize = sizeof(locoKeys) / sizeof(locoKeys[0]);
