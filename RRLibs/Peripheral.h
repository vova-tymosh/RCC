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
