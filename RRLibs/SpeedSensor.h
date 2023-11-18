/*
 * Speed Sensor (rotation counter)
 * 
 * 
 * 
 */
#include <SimpleKalmanFilter.h>


class SpeedSensor {
  private:
    const long MAX_PERIOD = 3000;
    SimpleKalmanFilter simpleKalmanFilter;
    int pin;
    double wheelDia;
    double clicksPerRotation;
    double speed;
    double rotationPerSecond;
  public:
    SpeedSensor(int _pin, double _clicksPerRotation, double _wheelDia) : 
      pin(_pin), clicksPerRotation(_clicksPerRotation), wheelDia(_wheelDia), simpleKalmanFilter(5, 5, 0.5) {
    }
    float getSpeed() {
      return this->speed;
    }
    void setup();
    void loop();
};
