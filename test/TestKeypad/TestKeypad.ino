#include "RCCKeypad.h"
#include "Storage.h"
#include "Settings.h"
#include "Timer.h"



Storage storage;
Settings settings;

class TestKeypad : public RCCKeypad 
{
};
TestKeypad keypad;



void setup()
{
    Serial.begin(115200);
    Serial.println("Started");
    storage.begin();
    settings.defaults(keypadKeys, keypadValues, keypadKeySize);


    keypad.debugLevel = 10;
    keypad.setup();
}

void loop()
{
    keypad.loop();

    // if () {
    //     update = true;
    //     controls.direction = loco.direction;
    // }

    
            // comms.send('t', (float)controls.throttle);

}
 
