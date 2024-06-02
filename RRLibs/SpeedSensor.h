/*
 * Speed Sensor (rotation counter)
 * 
 * 
 * 
 */
#include "Timer.h"
#include <SimpleKalmanFilter.h>


void speedHandler();

class SpeedSensorBase {
  public:
    virtual void update();
};

class SpeedSensor: public SpeedSensorBase {
  private:
    const int updatePeriod;
    const float distancePerClick;
    const int pin;
    Timer timer;
    float distance;
    float lastDisatnce;
    unsigned long lastTime;
    float speed;
  public:
    SpeedSensor(int pin, float distancePerClick, int updatePeriod = 500) :
      pin(pin), distancePerClick(distancePerClick), updatePeriod(updatePeriod) {
    }
    void setup() {
      pinMode(pin, INPUT);
      attachInterrupt(digitalPinToInterrupt(pin), speedHandler, RISING);
      lastTime = millis();
      timer.start(updatePeriod);
    }
    void loop() {
      if (timer.hasFired()) {
        unsigned long now = millis();
        speed = (float)(distance - lastDisatnce) / ((float)(now - lastTime) / 1000);
        lastDisatnce = distance;
        lastTime = now;
      }
    }
    void update() {
      distance += distancePerClick;
    }
    float getDistance() {
      return distance;
    }
    float getSpeed() {
      return speed;
    }
};

class SpeedSensorLegacy: public SpeedSensorBase {
  private:
    const long MAX_PERIOD = 3000;
    const int DEBOUNCE_THRESHOLD = 20;
    const double SCALE = 2;
    SimpleKalmanFilter filter;
    int pin;

    volatile long period;
    volatile long lastTimestamp;

    double speed;
    double rotationPerSecond;

  public:
    double measurementError;
    SpeedSensorLegacy(int pin) : pin(pin), filter(5, 5, 0.5) {
    }
    void setup() {
      pinMode(pin, INPUT_PULLUP);
      attachInterrupt(digitalPinToInterrupt(pin), speedHandler, FALLING);
    }
    void setKalmanError(double value) {
      measurementError = value;
      filter.setMeasurementError(value);
    }
    void setKalmanNoise(double value) {
      filter.setProcessNoise(value);
    }
    float getSpeedRaw() {
      return rotationPerSecond;
    }
    float getSpeed() {
      return speed;
    }
    void loop() {
      volatile long localPeriod = period;
      if ( (millis() - lastTimestamp) < min(localPeriod * 2, MAX_PERIOD) )
        rotationPerSecond = SCALE * 1000.0 / localPeriod;
      else
        rotationPerSecond = 0;
      speed = filter.updateEstimate(rotationPerSecond);
    }
    void update() {
      long now = millis();
      uint16_t since = now - lastTimestamp;
      if (since > DEBOUNCE_THRESHOLD) {
        period = since;
        lastTimestamp = now;
      }
    }
};

extern SpeedSensorBase *speedSensorPtr;

void speedHandler() {
  speedSensorPtr->update();
}
