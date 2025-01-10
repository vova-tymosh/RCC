/*
 * RCC Example Locomotive
 *
 *
 */
#include "Timer.h"
// #include "Secrets.h"


// #define RCC_NO_STATION
// #define RCC_WIFI_AP
// #include "RCCLoco.h"
#include "Storage.h"

Storage storage;


Timer initit;
Timer doit;

/*
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
TestLoco loco(&s);
*/

char testStr[] = "abcdefghijklmnopqrstuvwxyz 1234567890 abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char buffer[64];

void test01()
{
    storage.write("test", testStr, sizeof(testStr));

    int s = storage.read("test", buffer, 20);
    buffer[s - 1] = '\0';
    Serial.print(buffer);
    if (strncmp(testStr, buffer, s) == 0)
        Serial.println("test1 ... ok");
}


void setup()
{
    Serial.begin(115200);
    storage.begin();
    delay(100);

    // loco.debugLevel = 1;
    // loco.setup();
    doit.start(1000);
}

void loop()
{
    // loco.loop();

    if (doit.hasFired()) {
        test01();
    }
}
