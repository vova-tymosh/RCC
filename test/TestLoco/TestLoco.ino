/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
#define RCC_DEBUG 1
#include "Motherboard.h"
#include "Peripheral.h"
#include "Settings.h"
#include "RCCLoco.h"

#include "Audio.h"
#include "TestStorage.h"
#include "TestPing.h"
#include "Defaults.h"



Storage storage;
Settings settings;
PinExt blue(0);
PinExt q(5);
StatusLed statusLed(q, 500);
PowerMeter powerMeter;
Motor motor(PIN_MOTOR_BCK, PIN_MOTOR_FWD);
Audio audio;
Timer update(1000);


class TestLoco : public RCCLoco
{
public:
    using RCCLoco::RCCLoco;
    static const int MAX_CHANNELS = Audio::MAX_CHANNELS;

    // Ping ping;
    uint8_t volume;

    uint8_t channels[MAX_CHANNELS];

    void onFunction(uint8_t code, bool value)
    {
        Serial.print("Function: "); Serial.print(code); Serial.println( value ? " ON" : " OFF");

        char actionName[4];
        snprintf(actionName, 4, "%d", code);
        int action = settings.getCachedInt(actionName);
        int pin = action & 0x07;
        Serial.print("  Pin: "); Serial.println(pin);
        bool loop = (action & 0x10) != 0;

        const char *fname = functions.idToName(code);
        if (loop) {
            if (value) {
                processPlay(fname, true, code);
            } else {
                stopSound(code);
            }
        } else {
            processPlay(fname, false);
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

    int processPlay(const char* cmd, bool loop = false, uint8_t functionCode = 255)
    {
        int channel = -1;
        const char *fileName = cmd;
        String path = storage.addFolder(SOUNDS_PATH, fileName);
        if (storage.exists(path.c_str())) {
            Serial.print("Play ");
            Serial.println(path);
            channel = audio.play(path, volume, loop);

            if (channel >= 0 && functionCode != 255) {
                channels[channel] = functionCode;
            }
        } else {
            Serial.print("No file ");
            Serial.println(path);
        }
        return channel;
    }

    void stopSound(uint8_t functionCode)
    {
        // Find and stop the channel playing this function
        for (int i = 0; i < MAX_CHANNELS; i++) {
            if (channels[i] == functionCode) {
                Serial.print("Stop channel ");
                Serial.println(channels[i]);
                audio.stop(i);
                channels[i] = -1;
            }
        }
    }

    void onCommand(uint8_t code, char* value, uint8_t size)
    {
        switch (code) {
        case 'B':
            // Serial.println(motor.readBemf());
            statusLed.blink(5);
            break;
        case 'P':
            processPlay(value);
            break;
        case 'V':
            volume = atoi(value);
            break;
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

    storage.begin();
    settings.begin(settingsArray, sizeofarray(settingsArray));
    motor.begin();
    blue.begin();
    powerMeter.begin();
    update.start();
    audio.begin(16000);
    // q.begin();
    // statusLed.begin(true);

    loco.begin();
    loco.volume = 255;

    // Initialize function channel tracking
    for (int i = 0; i < TestLoco::MAX_CHANNELS; i++) {
        loco.channels[i] = -1;
    }
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
        loco.state.battery = powerMeter.readVoltage();
        loco.state.current = powerMeter.readCurrent();
        loco.state.temperature = 0;
        loco.state.psi = 0;
    }
}



/* For future use, maybe */

/*
// defined(CONFIG_IDF_TARGET_ESP32C3)
// defined(CONFIG_IDF_TARGET_ESP32C6)

//   pinMode(WIFI_ENABLE, OUTPUT); // pinMode(3, OUTPUT);
//   digitalWrite(WIFI_ENABLE, LOW); // digitalWrite(3, LOW); // Activate RF switch control

//   delay(100);

//   pinMode(WIFI_ANT_CONFIG, OUTPUT); // pinMode(14, OUTPUT);
//   digitalWrite(WIFI_ANT_CONFIG, HIGH); // digitalWrite(14, HIGH); // Use external antenna



*/