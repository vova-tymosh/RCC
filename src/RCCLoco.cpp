/*
 * Railroad communication
 *
 *
 */
#include <Arduino.h>

extern const int locoKeySize = 12;

extern const char *locoKeys[];
const char *locoKeys[locoKeySize] = {
    "wifiap", "wifissid",   "wifipwd",      "loconame",    "locoaddr",
    "broker", "brokerport", "acceleration", "managespeed", "heartbeat", 
    "testvalue", "mqtt"};

extern const char *locoValues[];
const char *locoValues[locoKeySize] = {"ON", "RCC_Loco",      "RCC_Loco", "RCC",
                            "3",  "192.168.20.61", "1883",     "0",
                            "0",  "1000", "1.1", "ON"};

