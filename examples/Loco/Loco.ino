/*
 * RCC Example Locomotive
 *
 *
 */
#include "Peripheral.h"
#include "RCCLoco.h"
#include "SpeedSensor.h"
#include "Timer.h"

#if defined(ARDUINO_AVR_NANO)
    #define CE_PIN 10
    #define CSN_PIN 9
#elif defined(ARDUINO_AVR_LEONARDO)
    #define CE_PIN 19
    #define CSN_PIN 18
#elif defined(ARDUINO_ARCH_NRF52)
    #define CE_PIN 0
    #define CSN_PIN 0
#endif

const int NODE = 01;
const char *NAME = "t001";
Storage storage;
Wireless wireless(CE_PIN, CSN_PIN);
const float inches_per_click = 1;
SpeedSensor speed_sensor(7, inches_per_click);
SpeedSensorBase *speed_sensor_ptr = &speed_sensor;
Timer timer;
Light led(2);

class TestLoco : public RCCLoco
{
public:
    using RCCLoco::RCCLoco;

    void onFunction(char code, bool value)
    {
        led.apply(state.lights);
        Serial.println("Function: " + String((int)code) + "/" + String(value));
    }

    void onThrottle(uint8_t direction, uint8_t throttle)
    {
        state.speed = throttle;
        // state.distance += state.speed * 10;
    }
};
TestLoco loco(&wireless, NODE, NAME, &storage);

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
    storage.setup(VERSION);
    loco.setup();
    loco.debug = true;
    speed_sensor.setup();
    led.setup();
    led.apply(loco.state.lights);
    timer.start(100);
}

void loop()
{
    loco.loop();
    speed_sensor.loop();

    if (timer.hasFired()) {
        // loco.state.speed = speedSensor.getSpeed();
        loco.state.distance = speed_sensor.getDistance();
        loco.state.battery = 71;
        loco.state.temperature = 82;
        loco.state.psi = 23;
        if (loco.debug) {
            Serial.println("Loco:" + String(loco.state.throttle) + " " +
                        String(loco.state.direction) + " " +
                        String(loco.state.speed) + " " +
                        String(loco.state.distance) + " " + String(loco.pid.p) +
                        " " + String(loco.pid.i) + " " + String(loco.pid.d) +
                        " " + String(loco.pid.upper));
        }
    }
}
