#include "RCCKeypad.h"
#include "Storage.h"
#include "Settings.h"
#include "Timer.h"



Storage storage;
Settings settings;

class TestKeypad : public RCCKeypad 
{
public:

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
        case 'G':
            processGet(value);
            break;
        case 'S':
            processSet(value);
            break;
        }
    }
};
TestKeypad keypad;



void setup()
{
    Serial.begin(115200);
    Serial.println("Started");
    storage.begin();
    // settings.defaults(keypadKeys, keypadValues, keypadKeySize);


    keypad.debugLevel = 10;
    keypad.setup();
}

void loop()
{
    keypad.loop();


}
 
