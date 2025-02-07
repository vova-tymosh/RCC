/*
 * RCC Example Locomotive
 *
 *
 */


#define RCC_NO_STATION
#include "RCCLoco.h"
#include "Timer.h"
#include "Storage.h"
#include "Settings.h"
#include "Motherboard.h"


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


Storage storage;
Settings settings;
Timer testRun, timer;
Cli cli(&loco);

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
    if (strcmp(testStr + 2, buffer) == 0)
        Serial.println("test2 ... ok");
}

void test03()
{
    storage.write("test03", testStr, sizeof(testStr));

    int s = storage.read("test02", buffer, 12, 10);
    buffer[s + 1] = '\0';
    Serial.println(buffer);
    if (strcmp(testStr + 10, buffer) == 0)
        Serial.println("test3 ... ok");
}

void test04()
{
    int s = storage.read("test01", buffer, 12);
    buffer[s + 1] = '\0';
    Serial.println(buffer);
    if (strcmp(testStr, buffer) == 0)
        Serial.println("test4 ... ok");
}

void test05()
{
    // settings.put("test05", testStr);
    String r = settings.get("test05");
    Serial.println(r);
    if (strcmp(testStr, r.c_str()) == 0)
        Serial.println("test5 ... ok");
}

void setup()
{
    Serial.begin(115200);
    delay(100);
    storage.begin();
    // storage.clear();
    settings.checkDefaults(defaultSettings, defaultSettingsSize);

    loco.debugLevel = 2;
    loco.setup();

    testRun.start(2000);
}

void loop()
{
    loco.loop();
    cli.loop();

    if (testRun.hasFiredOnce()) {
        Serial.println("-----------------------------");
        settings.put("test05", testStr);
        test03();
        test05();
        String name = settings.get("loconame");
        Serial.println(">>" + name);
            
    //     String wifiap = settings.get("wifiap");
    //     String wifissid = settings.get("wifissid");
    //     String wifipwd = settings.get("wifipwd");
    //     Serial.println("wifiap: " + wifiap);
    //     Serial.println("wifissid: " + wifissid);
    //     Serial.println("wifipwd: " + wifipwd);
    }

    // if (timer.hasFired()) {
    //     Serial.println("002 Tender:" + 
    //                           + " " + String(loco.state.throttle)   + " " +
    //                           String(loco.state.distance)
    //                           + " " + String(sensors.temperature) + " " +
    //                           String(sensors.psi));        
    // }
}
