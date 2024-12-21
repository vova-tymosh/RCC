/*
 * RCC Example Locomotive
 *
 *
 */
#include "RCCLoco.h"
#include "Timer.h"
#include "Secrets.h"

#define RCC_NO_STATION
#include "Transport.h"



void connectToWifi()
{
    WiFi.begin(SECRET_SSID, SECRET_PWD);
    int start = millis();
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    int connectionTime = millis() - start;
    Serial.print("\nConnected in: ");
    Serial.print(connectionTime/1000);
    Serial.println("s");
}



class TestLoco : public RCCLoco
{
public:
    using RCCLoco::RCCLoco;

    void onFunction(char code, bool value)
    {
        Serial.println("Function: " + String((int)code) + "/" + String(value));
    }

    void onThrottle(uint8_t direction, uint8_t throttle)
    {
        state.speed = throttle;
    }
};
TestLoco loco;
Transport transport(&loco);



void setupSerial()
{
    Serial.begin(115200);
    delay(50);
    // for (int i = 0; i < 100 || !Serial; i++);
    Serial.println(F("Started"));
}

void setup()
{
    setupSerial();
    connectToWifi();
    transport.begin();
    loco.setup();

    // loco.debug = true;
}

void loop()
{
    loco.loop();
    transport.loop();
}
