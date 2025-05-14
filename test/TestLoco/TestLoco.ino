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
#include "TestStorage.h"



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
TestLoco loco;



void setup()
{
    Serial.begin(115200);
    delay(250);
    // while ( !Serial ) delay(10);
    // Serial.println("Enter to any keys to continue:");
    // while ( !Serial.available() )
    // {
    //     delay(1);
    // }
    // Serial.println("Start");

  
    storage.begin();
    settings.begin(locoKeys, locoValues, locoKeySize);
    motor.begin();
    yellow.begin();
    blue.begin();
    led.begin();
    powerMeter.begin();
    timer.start(100);
    timer2.start(100);
    blinker.start();
    update.start();
    
    audio.begin();
    loco.debugLevel = 10;
    loco.begin();
}

void loop()
{
    loco.loop();
    // audio.loop();

    if (update.hasFired()) {
        loco.state.battery = powerMeter.readBattery();
        loco.state.current = powerMeter.readCurrent();
        loco.state.distance = 101;
        loco.state.speed = 20;
        loco.state.temperature = 110;
        loco.state.psi = 35;
    }

    // if (blinker.hasFired()) {
    //     static bool flip = false;
    //     flip = !flip;
    //     if (flip) {
    //         // digitalWrite(LED_BUILTIN, HIGH);
    //     } else {
    //         // digitalWrite(LED_BUILTIN, LOW);
    //     }
    // }

    // if (ledEnable && timer2.hasFired()) {
    //     static bool flip;
    //     Serial.print(">>");
    //     Serial.println(flip);
    //     led.apply(flip);
    //     flip = !flip;
    // }

}

