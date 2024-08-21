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
  protected:
    const float MIN_THR = 10;
    const int pin_back;
    const int pin_fowd;

    virtual void stop() {
      digitalWrite(pin_back, LOW);
      digitalWrite(pin_fowd, LOW);
    }

    virtual void forward(int throttle) {
      digitalWrite(pin_back, LOW);
      analogWrite(pin_fowd, throttle);
    }

    virtual void backward(int throttle) {
        analogWrite(pin_back, throttle);
        digitalWrite(pin_fowd, LOW);
    }

  public:
    Motor(int pin_back, int pin_fowd): pin_back(pin_back), pin_fowd(pin_fowd) {}

    virtual void setup() {
      pinMode(pin_back, OUTPUT);
      pinMode(pin_fowd, OUTPUT);
    }

    virtual void apply(int direction, int throttle) {
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

class ThermoSensor2 {
  private:
    const int PIN_VCC = 10;
    const int PIN_GND = A1;
    int pin;

    const float KELVIN_BASE = 273.15;
    const unsigned int THERMISTOR_RT0 = 10000;
    const unsigned int THERMISTOR_B = 3977;
    const float THERMISTOR_T0 = 25 + KELVIN_BASE;

    const float VCC = 5.0;
    const int MAX_ADC = 1023;
    const int RESISTOR = 10000;

    float getThermistor() {
      pinMode(PIN_VCC, OUTPUT);
      digitalWrite(PIN_VCC, HIGH);
      pinMode(PIN_GND, OUTPUT);
      digitalWrite(PIN_GND, LOW);

      pinMode(pin, INPUT);
      float voltage = analogRead(pin);
      pinMode(pin, OUTPUT);
      return (VCC / MAX_ADC) * voltage ;
    }

    float getTemperature() {
      float thermistorVoltage = getThermistor();
      float vr = VCC - thermistorVoltage;
      float resistance = thermistorVoltage / (vr / RESISTOR);

      float ln = log( resistance / THERMISTOR_RT0 );
      float temperature = (1 / ((ln / THERMISTOR_B) + (1 / THERMISTOR_T0)));
      return temperature - KELVIN_BASE;
    }
  public:
    ThermoSensor2(int pin): pin(pin) {};

    void setup() {
      pinMode(PIN_VCC, OUTPUT);
      digitalWrite(PIN_VCC, HIGH);
      pinMode(PIN_GND, OUTPUT);
      digitalWrite(PIN_VCC, LOW);
    }

    float read() {
      return getTemperature();
    }
};


class ThermoSensor {
  private:
    const int THERMISTOR_B = 3977;
    const int THERMISTOR_T0 = 25;
    const int THERMISTOR_RT0 = 10000;
    const int EXTERNAL_RESISTOR = 10000;
    const float KELVIN_BASE = 273.15;
    int pin;

  public:
    ThermoSensor(const int pin): pin(pin) {}

    void setup() {
      pinMode(pin, INPUT);
    }

    float read() {
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

class ThermoSensorPowered: public ThermoSensor {
  protected:
    const int vcc;
    const int gnd;

  public:
    ThermoSensorPowered(const int pin, const int vcc, const int gnd): ThermoSensor(pin), vcc(vcc), gnd(gnd) {}

    void setup() {
      ThermoSensor::setup();
      pinMode(vcc, OUTPUT);
      digitalWrite(vcc, HIGH);
      pinMode(gnd, OUTPUT);
      digitalWrite(gnd, LOW);
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
