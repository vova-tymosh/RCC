/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
#include "RCCKeypad.h"
#include "Storage.h"
#include "Settings.h"
#include "Timer.h"



Storage storage;
Settings settings;

class TestKeypad : public RCCKeypad 
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
            cycleSelected();
            break;
        }
    }
};
TestKeypad keypad;


const char *padKeys[] =   {"loconame",   "locoaddr", "testvalue"};
const char *padValues[] = {"RCC_Keypad", "1",        "0.0"};


void setup()
{
    Serial.begin(115200);
    Serial.println("Started");
    storage.begin();
    settings.begin(padKeys, padValues, sizeof(padKeys) / sizeof(padKeys[0]));

    keypad.begin();
}

void loop()
{
    keypad.loop();
}
 
