/*
 * RCC Example Locomotive
 *
 *
 */
#include <WiFi.h>
#include "Timer.h"
#include "Secrets.h"

#define RCC_NO_STATION
#include "RCCLoco.h"




void connectToWifi()
{
    WiFi.begin(SECRET_SSID, SECRET_PWD);
    int start = millis();
    Serial.print("Connecting to wifi.");
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
    }
    int connectionTime = millis() - start;
    Serial.print(" Done in ");
    Serial.print((float)connectionTime/1000);
    Serial.println("s");
}



class TestLoco : public RCCLoco
{
public:
    using RCCLoco::RCCLoco;

    void onFunction(bool activate, uint8_t code)
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
    connectToWifi();

    loco.debugLevel = 1;
    loco.setup();
}

void loop()
{
    loco.loop();
}
