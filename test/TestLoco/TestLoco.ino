/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */

// #define RCC_DEBUG 2
#include "Motherboard.h"
#include "Peripheral.h"
#include "Settings.h"
#include "RCCLoco.h"

#include "Audio.h"
// #include "audio_data3.h"
#include "TestStorage.h"
#include "TestPing.h"


// #include "toto1616.h"
// #include "tone.h"



// defined(CONFIG_IDF_TARGET_ESP32C3)
// defined(CONFIG_IDF_TARGET_ESP32C6)

Storage storage;
Settings settings;
// PinExt yellow(2);
PinExt blue(0);

PinExt q(5);
StatusLed statusLed(q, 500);

PowerMeter powerMeter;
Motor motor(PIN_MOTOR_BCK, PIN_MOTOR_FWD);
Timer update(1000);
Audio audio;

const KeyValue settingsArray[] = {
    {"wifiap",       "ON"},
    {"wifissid",     "RCC_Loco"},
    {"wifipwd",      "RCC_Loco"},
    {"loconame",     "RCC"},
    {"locoaddr",     "3"},
    {"broker",       "192.168.0.10"},
    {"brokerport",   "1883"},
    {"acceleration", "0"},
    {"managespeed",  "0"},
    {"heartbeat",    "1000"},
    {"testvalue",    "1.1"},
    {"mqtt",         "ON"},
    {"pump",         "0"}
};


const int PAGE_SIZE = 4096;
uint8_t page[PAGE_SIZE];
char soundFile1616[] = "/sound1616";
// char soundFile168[] = "/sound168";
char soundFileTone[] = "/soundTone";

void writeAllAudio(const char *filename, const uint8_t *data, const size_t size) {
    uint32_t offset = 0;

    while (offset < size) {
        uint32_t s = size - offset;
        if (s > sizeof(page))
            s = sizeof(page);
        memcpy(page, data + offset, s);
        int r = storage.write(filename, page, s, offset);
        offset += r;
    }
}


class TestLoco : public RCCLoco
{
public:
    using RCCLoco::RCCLoco;
    // Ping ping;


    void onFunction(uint8_t code, bool value)
    {
        Serial.print("Function: "); Serial.print(code); Serial.println( value ? " ON" : " OFF");

        if (code == 0) {
            blue.apply(value);
            audio.play(soundFile1616);
        }
    }

    void onThrottle(uint8_t direction, uint8_t throttle)
    {
        Serial.print("Throttle: "); Serial.print(throttle); Serial.print(" D:"); Serial.println(direction); 
        motor.apply(direction, throttle);
    }
    
    void onConnect(uint8_t connType)
    {
        statusLed.blink(connType);
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
            // Serial.println(motor.readBemf());
            statusLed.blink(5);
            break;
        case '3':
            audio.play(soundFile1616);
            break;
        case '4':
            audio.play(soundFile1616, 3);
            break;
        case '5':
            audio.play(soundFileTone);
            break;
        case 'L':
            audio.cycle = true;
            break;
        // case '7':
        //     Serial.print("Writing audio to disc tone...");
        //     writeAllAudio(soundFileTone, (uint8_t*)tone440, sizeof(tone440));
        //     Serial.println("done");
        //     break;
        // case '9':
        //     Serial.print("Writing audio to disc 1616...");
        //     writeAllAudio(soundFile1616, (uint8_t*)audio_data1616, sizeof(audio_data1616));
        //     Serial.println("done");
        //     break;
        case 'C':
            processCreate(value);
            break;
        case 'D':
            storage.begin(0);
            storage.begin();
            settings.begin(settingsArray, sizeofarray(settingsArray));
            Serial.println("Clear");
            break;
        case 'T':
            if (size > 0) {
                int idx = atoi(value);
                if (idx < (sizeof(tests) / sizeof(tests[0])))
                    tests[idx]();
            }
            break;
        case 'Z':
            setValue("heartbeat", "0");
            // ping.begin(transport);
            break;
        case 'X':
            setValue("heartbeat", "1000");
            // ping.end();
            break;
        case NRF_PING:
            // ping.receive(value, size);
            break;
        }
    }
};
TestLoco loco;



void setup()
{
    Serial.begin(115200);
    delay(50);

//   pinMode(WIFI_ENABLE, OUTPUT); // pinMode(3, OUTPUT);
//   digitalWrite(WIFI_ENABLE, LOW); // digitalWrite(3, LOW); // Activate RF switch control

//   delay(100);

//   pinMode(WIFI_ANT_CONFIG, OUTPUT); // pinMode(14, OUTPUT);
//   digitalWrite(WIFI_ANT_CONFIG, HIGH); // digitalWrite(14, HIGH); // Use external antenna


    storage.begin();
    settings.begin(settingsArray, sizeofarray(settingsArray));
    motor.begin();
    // yellow.begin();
    blue.begin();
    powerMeter.begin();
    update.start();
    audio.begin(16000);
    // q.begin();
    // statusLed.begin(true);

    loco.begin();
}


int getRand()
{
    static float angle = 0;
    float value = (sin(radians(angle)) + 1) * 50;
    angle += 70;
    if (angle >= 360) angle = 0;
    return (int)value;
}

void loop()
{
    loco.loop();
    audio.loop();
    // statusLed.loop();
    // loco.ping.loop();

    if (update.hasFired()) {
        static int d = 0;
        int r = getRand();
        loco.state.distance = d++;
        loco.state.speed = (r + 50)/3;
        loco.state.lost = 20;
        loco.state.battery = r;
        loco.state.current = (r + 20);
        loco.state.temperature = r + 50;
        loco.state.psi = r + 35;
    }
}

