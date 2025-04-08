#pragma once

#include <TCA6408A.h>
#include <Adafruit_INA219.h>
#include "Peripheral.h"

#define PIN_NOTHING   D0
#define PIN_MOTOR_EMF D1
#define PIN_MOTOR_BCK D2
#define PIN_MOTOR_FWD D3
#define PIN_SPEED_PIN D6

#if defined(ARDUINO_ARCH_NRF52)
#define PIN_AUDIO_OUT 32 + 12 // D7
#define PIN_AUDIO_BCK 32 + 13 // D8
#define PIN_AUDIO_LRC 32 + 14 // D9
#elif defined(ARDUINO_ARCH_ESP32)
#define PIN_AUDIO_OUT D7
#define PIN_AUDIO_BCK D8
#define PIN_AUDIO_LRC D9
#endif

extern const char *settingsKeys[];
extern const char *settingsValues[];
extern const int settingsSize;

class PinExt : public Pin, protected Tca6408a
{
public:
    PinExt(int pin, bool alternativeAddr = false)
        : Pin(pin), Tca6408a(alternativeAddr) {};

    void begin()
    {
        if (!Tca6408a::begin()) {
            Serial.println("Failed to find TCA6408A chip");
            return;
        }
        Tca6408a::setOutput(1 << pin);
    }

    void apply(bool on)
    {
        Tca6408a::setValue(1 << pin, on);
    }
};

class PinInputExt : protected Tca6408a
{
    uint8_t pin;

public:
    PinInputExt(int pin, bool alternativeAddr = false)
        : pin(pin), Tca6408a(alternativeAddr) {};

    void begin()
    {
        Tca6408a::begin();
    }

    bool read()
    {
        return Tca6408a::getValue(1 << pin);
    }
};

class PowerMeter
{
private:
    bool active = false;

public:
    Adafruit_INA219 ina219;

    PowerMeter() : ina219() {}
    void setup()
    {
        if (ina219.begin()) {
            ina219.setCalibration_32V_2A();
            active = true;
        } else {
            Serial.println("Failed to find INA219 chip");
        }
    }
    float readVoltage()
    {
        return active ? ina219.getBusVoltage_V() : 0;
    }
    float readCurrent()
    {
        return active ? ina219.getCurrent_mA() : 0;
    }
    float readPower()
    {
        return active ? ina219.getPower_mW() : 0;
    }
};
