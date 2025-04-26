/*
 * RCC Example Locomotive
 *
 *
 */
// #define RCC_NO_STATION
#include "Motherboard.h"
#include "RCCLoco.h"
#include "Audio.h"
#include "audio_data2.h"



// defined(CONFIG_IDF_TARGET_ESP32C3)
// defined(CONFIG_IDF_TARGET_ESP32C6)

Storage storage;
Settings settings;
PinExt yellow(2);
PinExt led(5);
Pin blue(D0);
PowerMeter powerMeter;
Motor motor(PIN_MOTOR_BCK, PIN_MOTOR_FWD);
Timer timer;
Timer blinker(1000);
Timer update(1000);
Timer timer2(10);


Audio audio;

bool ledEnable = false;

const int PAGE_SIZE = 256;
uint8_t page[PAGE_SIZE];
char soundFile[] = "sound";

void writeAllAudio(const uint8_t *data, const size_t size) {
    uint32_t offset = 0;
    storage.allocate(soundFile, size);

    while (offset < size) {
        uint32_t s = size - offset;
        if (s > sizeof(page))
            s = sizeof(page);
        memcpy(page, data + offset, s);
        int r = storage.write(soundFile, page, s, offset);
        offset += r;
    }
}

char testStr[] = "0123456789abcd";
char buffer[64];

void test01()
{
    storage.write("test01", testStr, sizeof(testStr));

    int s = storage.read("test01", buffer, 12);
    buffer[s + 1] = '\0';
    Serial.println(buffer);
    if (strncmp(testStr, buffer, s) == 0)
        Serial.println("test01 ... ok");
}

void test011()
{
    int s = storage.read("test01", buffer, 12);
    buffer[s + 1] = '\0';
    Serial.println(String("test011.") + String(s) + "=" + buffer);
    if (strcmp(testStr, buffer) == 0)
        Serial.println("test011 ... ok");
}


void test02()
{
    storage.write("test02", testStr, sizeof(testStr));

    int s = storage.read("test02", buffer, 12, 2);
    buffer[s + 1] = '\0';
    Serial.println(buffer);
    if (strcmp(testStr + 2, buffer) == 0)
        Serial.println("test02 ... ok");
}

void test03()
{
    storage.write("test03", testStr, sizeof(testStr));

    int s = storage.read("test02", buffer, 12, 10);
    buffer[s + 1] = '\0';
    Serial.println(buffer);
    if (strcmp(testStr + 10, buffer) == 0)
        Serial.println("test03 ... ok");
}

void test05()
{
    settings.put("test05", testStr);
    String r = settings.get("test05");
    Serial.println(r);
    if (strcmp(testStr, r.c_str()) == 0)
        Serial.println("test05 ... ok");
}

void test051()
{
    String r = settings.get("test05");
    Serial.println(r);
    if (strcmp(testStr, r.c_str()) == 0)
        Serial.println("test051 ... ok");
}

void test06()
{
    settings.put("name", "1204");
    String r = settings.get("name");
    Serial.println(r);
    if (strcmp("1204", r.c_str()) == 0)
        Serial.println("test06 ... ok");
}

void test061()
{
    String r = settings.get("name");
    Serial.println(r);
    if (strcmp("1204", r.c_str()) == 0)
        Serial.println("test061 ... ok");
}


class TestLoco : public RCCLoco
{
public:
    using RCCLoco::RCCLoco;

    void onFunction(uint8_t code, bool value)
    {
        Serial.println("onFunction: " + String(code) + "/" + String(value));
        if (code == 0)
            yellow.apply(value);
        if (code == 1)
            blue.apply(value);
    }

    void onThrottle(uint8_t direction, uint8_t throttle)
    {
        Serial.println("onThrottle: " + String(direction) + "/" + String(throttle));
        motor.apply(direction, throttle);
    }

    void onCommand(uint8_t code, char* value, uint8_t size)
    {
        switch (code) {
        case 'B':
            Serial.println(motor.readBemf());
            break;
        case 'P':
            audio.play(audio_data, sizeof(audio_data), 2);
            break;
        case 'Q':
            audio.play("sound", 2);
            break;
        case 'L':
            audio.cycle = true;
            break;
        case 'W':
            writeAllAudio(audio_data, sizeof(audio_data));
            break;
        case 'Z':
            ledEnable = true;
            break;
        }

    }
};
TestLoco loco;



void setup()
{
    Serial.begin(115200);
    delay(250);

    storage.begin();
    settings.defaults(locoKeys, locoValues, locoKeySize);

    motor.begin();
    yellow.begin();
    blue.begin();
    led.begin();
    powerMeter.begin();
    timer.start(100);
    timer2.start(100);
    blinker.start();
    
    audio.begin();
    loco.debugLevel = 10;
    loco.begin();
}

void loop()
{
    loco.loop();
    audio.loop();

    if (update.hasFired()) {
        loco.state.battery = powerMeter.readBattery();
        loco.state.current = powerMeter.readCurrent();
    }

    if (blinker.hasFired()) {
        static bool flip = false;
        flip = !flip;
        if (flip) {
            // digitalWrite(LED_BUILTIN, HIGH);
        } else {
            // digitalWrite(LED_BUILTIN, LOW);
        }
    }

    if (ledEnable && timer2.hasFired()) {
        static bool flip;
        Serial.print(">>");
        Serial.println(flip);
        led.apply(flip);
        flip = !flip;
    }

}

