/*
 * RCC Example Locomotive
 *
 *
 */
// #define RCC_DEBUG 2
#include "Motherboard.h"
#include "Peripheral.h"
#include "RCCLoco.h"
#include "Audio.h"
#include "audio_data2.h"
#include "TestStorage.h"



// defined(CONFIG_IDF_TARGET_ESP32C3)
// defined(CONFIG_IDF_TARGET_ESP32C6)

Storage storage;
Settings settings;
PinExt yellow(2);
Pin blue(0);


PowerMeter powerMeter;
Motor motor(PIN_MOTOR_BCK, PIN_MOTOR_FWD);
Timer update(1000);
Audio audio;

const int settingsSize = 13;
const char *settingsKeys[settingsSize] = {
    "wifiap",       "wifissid",   "wifipwd",      "loconame",    "locoaddr",
    "broker",       "brokerport", "acceleration", "managespeed", "heartbeat", 
    "testvalue",    "mqtt",       "pump"};
const char *settingsValues[settingsSize] = {
    "ON",           "RCC_Loco",   "RCC_Loco",     "RCC",         "3",
    "192.168.0.10", "1883",       "0",            "0",           "1000",
    "1.1",          "ON",         "0"};



const int PAGE_SIZE = 256;
uint8_t page[PAGE_SIZE];
char soundFile[] = "sound";
void writeAllAudio(const uint8_t *data, const size_t size) {
    uint32_t offset = 0;
    // storage.allocate(soundFile, size);

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
        Serial.print("Function: "); Serial.print(code); 
        if (value)
            Serial.println(" ON");
        else
            Serial.println(" OFF");
        if (code == 0)
            yellow.apply(value);
        if (code == 1)
            blue.apply(value);
    }

    void onThrottle(uint8_t direction, uint8_t throttle)
    {
        Serial.print("Throttle: "); Serial.print(throttle); Serial.print(" D:"); Serial.println(direction); 
        motor.apply(direction, throttle);
    }

    void processCreate(char cmd[])
    {
        if (strlen(cmd) < 2)
            return;
        char *separator = strchr(cmd, ':');
        if (separator == NULL)
            return;
        *separator = '\0';
        char *key = cmd;
        char *value = ++separator;
        settings.create(key, value);
        Serial.print(key);
        Serial.print(":");
        Serial.println(value);
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
        case 'C':
            processCreate(value);
            break;
        case 'D':
            storage.begin(0);
            storage.begin();
            settings.begin(settingsKeys, settingsValues, settingsSize);
            Serial.println("Clear");
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
    //     delay(1);
    // Serial.println("Start");

  
    storage.begin();
    settings.begin(settingsKeys, settingsValues, settingsSize);
    motor.begin();
    yellow.begin();
    blue.begin();
    powerMeter.begin();
    update.start();
    
    audio.begin();
    loco.begin();
}

void loop()
{
    loco.loop();
    audio.loop();

    if (update.hasFired()) {
        loco.state.battery = powerMeter.readBattery();
        loco.state.current = powerMeter.readCurrent();
        loco.state.distance = 101;
        loco.state.speed = 20;
        loco.state.temperature = 110;
        loco.state.psi = 35;
    }
}

