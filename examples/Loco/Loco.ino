/*
 * RCC Example Locomotive
 *
 *
 */
#include "Timer.h"
#include "Secrets.h"


#define RCC_NO_STATION
// #define RCC_WIFI_AP
#include "RCCLoco.h"



class TestLoco : public RCCLoco
{
public:
    using RCCLoco::RCCLoco;

    void onFunction(uint8_t code, bool activate)
    {
        log(String("Function: ") + code + ((activate) ? " ON" : " OFF"));
    }

    void onThrottle(uint8_t direction, uint8_t throttle)
    {
        log(String("Throttle: ") + throttle + " Dir: " + direction);
        state.speed = throttle;
    }
};
TestLoco loco;


void setup()
{
    Serial.begin(115200);
    delay(50);

    loco.debugLevel = 1;
    loco.setup();
}

void loop()
{
    loco.loop();
}
