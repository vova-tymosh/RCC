#pragma once
#include <Servo.h>


class Light {
    uint8_t pin;
  public:
    Light(int pin): pin(pin) {};
    void setup() {
      pinMode(pin, OUTPUT);
    }
    void apply(bool on) {
      if (on)
        digitalWrite(pin, HIGH);
      else
        digitalWrite(pin, LOW);
    }
};

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

class Motor {
    const float MIN_THR = 10;
    const int pin_back;
    const int pin_fowd;
  public:
    Motor(int pin_back, int pin_fowd): pin_back(pin_back), pin_fowd(pin_fowd) {}

    void setup() {
      pinMode(pin_back, OUTPUT);
      pinMode(pin_fowd, OUTPUT);
    }

    void apply(int direction, int throttle) {
      throttle = map(throttle, 0, 100, MIN_THR, 255);
      if (throttle == MIN_THR)
        throttle = 0;
      if (direction == 0) {
        digitalWrite(pin_back, LOW);
        digitalWrite(pin_fowd, LOW);
      } else if (direction == 1) {
        digitalWrite(pin_back, LOW);
        analogWrite(pin_fowd, throttle);
      } else {
        analogWrite(pin_back, throttle);
        digitalWrite(pin_fowd, LOW);
      }
    }
};

class TemperatureSensor {
  private:
    const int THERMISTOR_B = 3977;
    const int THERMISTOR_T0 = 25;
    const int THERMISTOR_RT0 = 10000;
    const int EXTERNAL_RESISTOR = 10000;
    const float KELVIN_BASE = 273.15;
    int pin;

  public:
    TemperatureSensor(const int pin): pin(pin) {}

    void setup() {
      pinMode(pin, INPUT);
    }

    float read() {
      float voltage = (float)1023 / analogRead(pin) - 1;
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

class PreassureSensor {
  private:
    int pin;

  public:
    PreassureSensor(int pin): pin(pin) {}
    void setup() {
      pinMode(pin, INPUT);
    }
    float read() {
      const float bitPerVolt = 1024 / 5;
      const float psiMax = 100;
      const float voltsMin = 0.48;
      const float voltsRange = 4.5 - voltsMin;
      const float bitsMin = voltsMin * bitPerVolt;
      float raw = analogRead(pin);
      return (raw - bitsMin) / (voltsRange * bitPerVolt / psiMax);
    }
};
