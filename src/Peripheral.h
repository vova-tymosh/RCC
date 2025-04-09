#pragma once
#include "Timer.h"

class Pin
{
protected:
    uint8_t pin;

public:
    Pin(int pin) : pin(pin) {};

    void begin()
    {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
    }

    void apply(bool on)
    {
        if (on)
            digitalWrite(pin, HIGH);
        else
            digitalWrite(pin, LOW);
    }
};

/*
class ServoBase {
  protected:
    int pin;
    Servo servo;
  public:
    ServoBase(int pin): pin(pin) {}

    void setup() {
      pinMode(pin, OUTPUT);
      servo.attach(pin);
    }

    virtual void apply(uint16_t value) {
      servo.write(value);
    }
};
*/

// TODO: Deprecate
class MotorDeprecated
{
protected:
    const float MIN_THR = 0;
    const int pin_back;
    const int pin_fowd;

    virtual void stop()
    {
        digitalWrite(pin_back, LOW);
        digitalWrite(pin_fowd, LOW);
    }

    virtual void forward(int throttle)
    {
        digitalWrite(pin_back, LOW);
        analogWrite(pin_fowd, throttle);
    }

    virtual void backward(int throttle)
    {
        analogWrite(pin_back, throttle);
        digitalWrite(pin_fowd, LOW);
    }

public:
    MotorDeprecated(int pin_back, int pin_fowd)
        : pin_back(pin_back), pin_fowd(pin_fowd)
    {
    }

    virtual void setup()
    {
        pinMode(pin_back, OUTPUT);
        pinMode(pin_fowd, OUTPUT);
    }

    virtual void apply(int direction, int throttle)
    {
        throttle = map(throttle, 0, 100, MIN_THR, 255);
        if (throttle == MIN_THR)
            throttle = 0;
        if (direction == 0) {
            stop();
        } else if (direction == 1) {
            forward(throttle);
        } else {
            backward(throttle);
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

    Motor(int pin_back, int pin_fowd, int pin_bemf = -1, int min_thr = 20,
          int cool_down_us = 40)
        : pin_back(pin_back), pin_fowd(pin_fowd), pin_bemf(pin_bemf),
          min_thr(min_thr), cool_down(cool_down_us)
    {
    }

    virtual void setup()
    {
        pinMode(pin_back, OUTPUT);
        pinMode(pin_fowd, OUTPUT);
        if (pin_bemf >= 0) {
            pinMode(pin_bemf, INPUT);
            bemf_timer.start(700);
        }
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
            analogWrite(pin_back, 0);
            analogWrite(pin_fowd, 0);
        } else if (direction == 1) {
            analogWrite(pin_back, MAX);
            analogWrite(pin_fowd, throttle);
        } else {
            analogWrite(pin_back, throttle);
            analogWrite(pin_fowd, MAX);
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

    void setup()
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

    void setup()
    {
        ThermoSensor::setup();
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
    void setup()
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
