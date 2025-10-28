/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
#define RCC_DEBUG 2
#include "RCCPad.h"
#include "Storage.h"
#include "Settings.h"
#include "Timer.h"



Storage storage;
Settings settings;
Timer debugTimer(1000);

class TestPad : public RCCPad 
{
public:

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

    void processGet(char cmd[])
    {
        if (strlen(cmd) < 1)
            return;
        char *key = cmd;
        String value = getValueLocal(key);
        Serial.println(String(key) + ":" + value);
        return;
    }

    void processSet(char* cmd)
    {
        if (strlen(cmd) < 2)
            return;
        char *separator = strchr(cmd, ':');
        if (separator == NULL)
            return;
        *separator = '\0';
        char *key = cmd;
        char *value = ++separator;
        setValueLocal(key, value);
    }

    void processNext(char* cmd)
    {
        if (strlen(cmd) == 0) {
            // No argument - cycle to next locomotive
            cycleSelected();
            Serial.print("Cycled to next loco: ");
            Serial.println(getSelectedName());
        } else {
            // Argument provided - try to select specific locomotive address
            uint8_t targetAddr = (uint8_t)atoi(cmd);
            if (selectByAddress(targetAddr)) {
                Serial.print("Selected loco address ");
                Serial.print(targetAddr);
                Serial.print(": ");
                Serial.println(getSelectedName());
            } else {
                Serial.print("Loco address ");
                Serial.print(targetAddr);
                Serial.println(" not found - ignoring command");
            }
        }
    }

    bool selectByAddress(uint8_t addr)
    {
        return selectLocoByAddress(addr);
    }

    virtual void onCommand(uint8_t code, char* value, uint8_t size)
    {
        value[size] = '\0';
        switch (code) {
        case 'C':
            processCreate(value);
            break;
        case 'G':
            processGet(value);
            break;
        case 'S':
            processSet(value);
            break;
        case 'N':
            processNext(value);
            break;
        }
    }
};
TestPad pad;


const KeyValue padSettings[] = {
    {"loconame",  "RCC_Pad"},
    {"locoaddr",  "1"},
    {"testvalue", "0.0"}
};


void setup()
{
    Serial.begin(115200);
    Serial.println("Started");
    storage.begin();
    settings.begin(padSettings, sizeofarray(padSettings));

    pad.begin();
    debugTimer.start();
}


void loop()
{
    pad.loop();
    // if (debugTimer.hasFired()) {
    //     Serial.print("Rate: ");
    //     Serial.println(pad.getConnSuccessRate());
    // }
}
 
