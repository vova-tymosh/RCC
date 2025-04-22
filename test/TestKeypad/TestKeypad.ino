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


const char *sk[] = {"loconame",     "locoaddr"};

const char *sv[] = {"RCC_Keypad",   "4"};

const int ss = sizeof(sk) / sizeof(sk[0]);


void setup()
{
    Serial.begin(115200);
    Serial.println("Started");
    storage.begin();
    settings.defaults(sk, sv, ss);


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
 
