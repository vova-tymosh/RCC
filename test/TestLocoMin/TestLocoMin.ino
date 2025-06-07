/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
#define NRF_CE 19
#define NRF_CSN 18

#include "Peripheral.h"
#include "RCCLoco.h"
#include "TestStorage.h"

Storage storage;
Settings settings;
Pin blue(0);
Timer update(1000);

const int settingsSize = 6;
const char *settingsKeys[settingsSize] = {
    "loconame", "locoaddr", "acceleration", "managespeed",  "heartbeat", "testvalue"};
const char *settingsValues[settingsSize] = {
    "RCC",      "3",        "0",            "0",            "1000",      "0.0"}; 


class TestLocoMin : public RCCLoco
{
public:
    using RCCLoco::RCCLoco;

    void onFunction(uint8_t code, bool value)
    {
        Serial.print(F("F ")); Serial.print(code); Serial.print(F("/")); Serial.println(value);
    }

    void onThrottle(uint8_t direction, uint8_t throttle)
    {
        Serial.print(F("T ")); Serial.print(direction); Serial.print(F("/")); Serial.println(throttle);
    }

    void processCreate(char cmd[])
    {
        if (strlen(cmd) < 2)
                return;
        char *separator = strchr(cmd, ':');
        if (separator == NULL)
            return;
        *separator = '\0';
        char *key = cmd;
        char *value = ++separator;
        settings.create(key, value);
        Serial.print(key);
        Serial.print(":");
        Serial.println(value);
    }

    void onCommand(uint8_t code, char* value, uint8_t size)
    {
        switch (code) {
        case 'C':
            processCreate(value);
            break;
        case 'D':
            storage.begin(0);
            storage.begin();
            settings.begin(settingsKeys, settingsValues, settingsSize);
            Serial.println("Clear");
            break;
        case 'T':
            if (size > 0) {
                int idx = atoi(value);
                if (idx < (sizeof(tests) / sizeof(tests[0])))
                    tests[idx]();
            }
            break;
        }

    }
};
TestLocoMin loco;



void setup()
{
    Serial.begin(115200);
    delay(250);

    // while ( !Serial ) delay(10);
    // Serial.println(F("Hit enter"));
    // while ( !Serial.available() )
    //     delay(1);
    // Serial.println("Start");
    // storage.clear();

  
    storage.begin();
    settings.begin(settingsKeys, settingsValues, settingsSize);

    loco.begin();
    update.start();
    Serial.println("start");
}

void loop()
{
    loco.loop();

    if (update.hasFired()) {
        loco.state.distance = 101;
        loco.state.speed = 20;
        loco.state.temperature = 110;
        loco.state.psi = 35;
    }
}

