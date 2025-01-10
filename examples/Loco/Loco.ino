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

char testStr[] = "0123456789abcdef";
char buffer[64];

void test01()
{
    storage.write("test01", testStr, sizeof(testStr));

    int s = storage.read("test01", buffer, 12);
    buffer[s + 1] = '\0';
    Serial.println(buffer);
    if (strncmp(testStr, buffer, s) == 0)
        Serial.println("test1 ... ok");
}

void test02()
{
    storage.write("test02", testStr, sizeof(testStr));

    int s = storage.read("test02", buffer, 12, 2);
    buffer[s + 1] = '\0';
    Serial.println(buffer);
    if (strncmp(testStr + 2, buffer, s) == 0)
        Serial.println("test2 ... ok");
}

void test03()
{
    storage.write("test03", testStr, sizeof(testStr));

    int s = storage.read("test02", buffer, 12, 10);
    buffer[s + 1] = '\0';
    Serial.println(buffer);
    if (strncmp(testStr + 10, buffer, s) == 0)
        Serial.println("test3 ... ok");
}

void test04()
{
    int s = storage.read("test01", buffer, 12);
    buffer[s + 1] = '\0';
    Serial.println(buffer);
    if (strncmp(testStr, buffer, s) == 0)
        Serial.println("test4 ... ok");
}

void setup()
{
    Serial.begin(115200);
    // storage.clear();
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
        Serial.println("-------");
        test01();
        test02();
        test03();
        test04();
    }
}
