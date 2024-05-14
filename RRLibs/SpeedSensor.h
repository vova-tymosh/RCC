/*
 * Speed Sensor (rotation counter)
 * 
 * 
 * 
 */
#include "Timer.h"


void speedHandler();

class SpeedSensor {
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

extern SpeedSensor speedSensor;

void speedHandler() {
  speedSensor.update();
}
