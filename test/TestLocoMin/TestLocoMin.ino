/*
 * RCC Example Locomotive
 *
 *
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

class TestLocoMin : public RCCLoco
{
public:
    using RCCLoco::RCCLoco;

    void onFunction(uint8_t code, bool value)
    {
        Serial.print(F("onFunction: ")); Serial.print(code); Serial.print(F("/")); Serial.println(value);
    }

    void onThrottle(uint8_t direction, uint8_t throttle)
    {
        Serial.print(F("onThrottle: ")); Serial.print(direction); Serial.print(F("/")); Serial.println(throttle);
    }

    void onCommand(uint8_t code, char* value, uint8_t size)
    {
        switch (code) {
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
    // Serial.println("Enter to any keys to continue:");
    // while ( !Serial.available() )
    //     delay(1);
    // Serial.println("Start");

  
    storage.begin();
    settings.begin(locoKeys, locoValues, locoKeySize, false);
    
    loco.debugLevel = 10;
    loco.begin();
    update.start();
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

