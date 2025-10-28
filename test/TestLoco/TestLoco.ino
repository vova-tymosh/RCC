/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */

#define RCC_DEBUG 2
#include "Motherboard.h"
#include "Peripheral.h"
#include "Settings.h"
#include "RCCLoco.h"

#include "Audio.h"
#include "audio_data2.h"
#include "TestStorage.h"



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

#if defined(ARDUINO_ARCH_NRF52)
class Ping
{
public:
    struct __attribute__((packed)) Packet {
        uint8_t code;
        uint16_t id;
        uint32_t ts;
        uint8_t pad[20];
    } packet;

    bool active = false;
    uint16_t recd = 0;
    uint16_t sent = 0;
    uint16_t sentLast = 0;
    uint16_t recdLast = 0;
    float avgTime = 0;
    const int maxCount = 20;
    Transport *transport;
    Timer printTimer;

    Timer shortTimer;
    Timer longTimer;


    void begin(Transport *transport)
    {
        this->transport = transport;
        active = true;
        sent = 0;
        recd = 0;
        shortTimer.start(100);
        longTimer.start(2090);
        printTimer.start(2090);
    }

    void end()
    {
        active = false;
    }

    void send()
    {
        packet.code = '0';
        packet.id = ++sent;
        packet.ts = millis();
        transport->write((uint8_t*)&packet, sizeof(packet));
    }

    void receive(char* value, uint8_t size)
    {
        if (size == sizeof(Packet)) {
            struct Packet *p = (struct Packet *)value;
            if (sent == p->id) {
                recd++;
                long time = millis() - p->ts;
                int c = (p->id > maxCount) ? maxCount : p->id;
                avgTime = (avgTime * c + time) / (c + 1);
                shortTimer.start();
            }
        }
    }

    void loop()
    {
        if (!active)
            return;

        if (shortTimer.hasFiredOnce()) {
            send();
            longTimer.start();
        }

        if (longTimer.hasFiredOnce()) {
            send();
            longTimer.start();
        }

        if (printTimer.hasFired()) {
            Serial.print("Ping avg: ");
            Serial.print(avgTime);
            Serial.print(", rate: ");
            Serial.print(recd - recdLast);
            Serial.print("/");
            Serial.println(sent - sentLast);
            sentLast = sent;
            recdLast = recd;
        }
    }
};
#endif

class TestLoco : public RCCLoco
{
public:
    using RCCLoco::RCCLoco;
    // Ping ping;


    void onFunction(uint8_t code, bool value)
    {
        Serial.print("Function: "); Serial.print(code); Serial.println( value ? " ON" : " OFF");

        if (code == 0)
            blue.apply(value);
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
        case 'P':
            // audio.play(audio_data, sizeof(audio_data), 2);
            q.apply(true);
            break;
        case 'Q':
            // audio.play("sound", 2);
            q.apply(false);
            break;
        case 'L':
            // audio.cycle = true;
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
    settings.begin(settingsKeys, settingsValues, settingsSize);
    motor.begin();
    // yellow.begin();
    blue.begin();
    powerMeter.begin();
    update.start();
    // audio.begin();
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
    // audio.loop();
    statusLed.loop();
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

