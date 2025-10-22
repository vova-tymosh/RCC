/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
#pragma once
#include "Timer.h"
#if defined(ARDUINO_ARCH_ESP32)
#include "driver/mcpwm.h"
#endif

class Pin
{
protected:
    uint8_t pin;

public:
    Pin(int pin) : pin(pin) {};

    virtual void begin()
    {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
    }

    virtual void apply(bool on)
    {
        if (on)
            digitalWrite(pin, HIGH);
        else
            digitalWrite(pin, LOW);
    }
};

class StatusLed
{
    Pin &pin;
    bool on = false;
    uint8_t blinks = 0;
    Timer timer;

public:
    StatusLed(Pin &pin, int period = 500) : pin(pin), timer(period) {}

    void begin(bool _on = false)
    {
        on = _on;
        pin.begin();
        pin.apply(on);
    }

    void blink(uint8_t _blinks = 1)
    {
        on = false;
        blinks = _blinks;
        pin.apply(on);
        if (blinks > 0) {
            timer.start();
        }
    }

    void loop()
    {
        if (timer.hasFiredOnce()) {
            on = !on;
            pin.apply(on);
            if (blinks > 0) {
                timer.start();
                if (on)
                    blinks--;
            }
        }
    }
};

class Motor
{
protected:
    const int pin_back;
    const int pin_fowd;
    const int pin_bemf;
    const int min_thr;
    const int cool_down;
    int direction;
    int throttle;
    Timer bemf_timer;

public:
    int bemf;

    Motor(int pin_back, int pin_fowd, int pin_bemf = -1, int min_thr = 20, int cool_down_us = 40)
        : pin_back(pin_back), pin_fowd(pin_fowd), pin_bemf(pin_bemf), min_thr(min_thr),
          cool_down(cool_down_us)
    {
    }

    virtual void begin()
    {
#if defined(ARDUINO_ARCH_ESP32)
        ledcAttach(pin_back, 50000, 8);
        ledcAttach(pin_fowd, 50000, 8);
#else
        pinMode(pin_back, OUTPUT);
        pinMode(pin_fowd, OUTPUT);
        if (pin_bemf >= 0) {
            pinMode(pin_bemf, INPUT);
            bemf_timer.start(700);
        }
#endif
        analogWrite(pin_back, 0);
        analogWrite(pin_fowd, 0);
    }

    virtual void apply(int direction, int throttle)
    {
        const uint8_t MAX = 0xFF;

        this->direction = direction;
        this->throttle = throttle;
        if (throttle > 0)
            throttle = map(throttle, 0, 100, MAX - min_thr, 0);
        else
            throttle = MAX;
        if (direction == 0) {
            analogWrite(pin_back, throttle);
            analogWrite(pin_fowd, MAX);
        } else if (direction == 1) {
            analogWrite(pin_back, MAX);
            analogWrite(pin_fowd, throttle);
        } else {
            analogWrite(pin_back, 0);
            analogWrite(pin_fowd, 0);
        }
    }

    int readBemf()
    {
        if (pin_bemf >= 0) {
            analogWrite(pin_back, 0);
            analogWrite(pin_fowd, 0);
            delayMicroseconds(cool_down);
            bemf = analogRead(pin_bemf);
            apply(direction, throttle);
            return bemf;
        } else {
            return -1;
        }
    }

    void loop()
    {
        if (bemf_timer.hasFired()) {
            readBemf();
        }
    }
};

class ThermoSensor
{
private:
    const int THERMISTOR_B = 3977;
    const int THERMISTOR_T0 = 25;
    const int THERMISTOR_RT0 = 10000;
    const int EXTERNAL_RESISTOR = 10000;
    const float KELVIN_BASE = 273.15;
    int pin;

public:
    ThermoSensor(const int pin) : pin(pin) {}

    void begin()
    {
        pinMode(pin, INPUT);
    }

    float read()
    {
        int reading = analogRead(pin);
        if (reading == 0)
            return 0;
        reading = constrain(reading, 0, 1022);
        float voltage = (float)1023 / reading - 1;
        float resistance = EXTERNAL_RESISTOR / voltage;
        float temperature = resistance / THERMISTOR_RT0;
        temperature = log(temperature);
        temperature /= THERMISTOR_B;
        temperature += 1.0 / (THERMISTOR_T0 + KELVIN_BASE);
        temperature = 1.0 / temperature;
        temperature -= KELVIN_BASE;
        return temperature;
    }
};

class ThermoSensorPowered : public ThermoSensor
{
protected:
    const int vcc;
    const int gnd;

public:
    ThermoSensorPowered(const int pin, const int vcc, const int gnd)
        : ThermoSensor(pin), vcc(vcc), gnd(gnd)
    {
    }

    void begin()
    {
        ThermoSensor::begin();
        pinMode(vcc, OUTPUT);
        digitalWrite(vcc, HIGH);
        pinMode(gnd, OUTPUT);
        digitalWrite(gnd, LOW);
    }
};

class PreassureSensor
{
private:
    int pin;
    int psiMax;

public:
    PreassureSensor(int pin, int psiMax = 100) : pin(pin), psiMax(psiMax) {}
    void begin()
    {
        pinMode(pin, INPUT);
    }
    float read()
    {
        const float bitPerVolt = 1024 / 5;
        const float voltsMin = 0.48;
        const float voltsRange = 4.5 - voltsMin;
        const float bitsMin = voltsMin * bitPerVolt;
        float raw = analogRead(pin);
        return (raw - bitsMin) / (voltsRange * bitPerVolt / psiMax);
    }
};
