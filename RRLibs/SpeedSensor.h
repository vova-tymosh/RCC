/*
 * Speed Sensor (rotation counter)
 * 
 * 
 * 
 */
#include <SimpleKalmanFilter.h>
#include "Timer.h"


class SpeedSensor {
  private:
    const long MAX_PERIOD = 1500;
    SimpleKalmanFilter simpleKalmanFilter;
    int pin;
    double wheelDia;
    double clicksPerRotation;
    double speed;
    long distance;
    double rotationPerSecond;
    Timer timer;
  public:
    SpeedSensor(int _pin, double _clicksPerRotation, double _wheelDia) : 
      pin(_pin), clicksPerRotation(_clicksPerRotation), wheelDia(_wheelDia), simpleKalmanFilter(1, 1, 0.1) {
    }
    float getSpeed() {
      return this->speed;
    }
    long getDistance() {
      return this->distance;
    }
    void setup();
    void loop();
};
