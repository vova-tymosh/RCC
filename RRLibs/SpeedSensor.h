/*
 * Speed Sensor (rotation counter)
 *
 *
 *
 */
#include "Timer.h"
#include <SimpleKalmanFilter.h>

void speedHandler();

class SpeedSensorBase
{
public:
    virtual void update();
};

class SpeedSensor : public SpeedSensorBase
{
private:
    const int update_period;
    const float distance_per_click;
    const int pin;
    Timer timer;
    float distance;
    float last_disatnce;
    unsigned long last_time;
    float speed;

public:
    SpeedSensor(int pin, float distance_per_click, int update_period = 500)
        : pin(pin), distance_per_click(distance_per_click),
          update_period(update_period) {}

    void setup()
    {
        pinMode(pin, INPUT);
        attachInterrupt(digitalPinToInterrupt(pin), speedHandler, RISING);
        last_time = millis();
        timer.start(update_period);
    }

    void loop()
    {
        if (timer.hasFired()) {
            unsigned long now = millis();
            speed = (float)(distance - last_disatnce) /
                    ((float)(now - last_time) / 1000);
            last_disatnce = distance;
            last_time = now;
        }
    }

    void update()
    {
        distance += distance_per_click;
    }

    float getDistance()
    {
        return distance;
    }

    float getSpeed()
    {
        return speed;
    }
};

class SpeedSensorLegacy : public SpeedSensorBase
{
private:
    const long MAX_PERIOD = 3000;
    const int DEBOUNCE_THRESHOLD = 20;
    const float SCALE = 2;
    SimpleKalmanFilter filter;
    int pin;

    volatile long period;
    volatile long lastTimestamp;

    float speed;
    float rotationPerSecond;
    float distance;

public:
    float measurementError;
    SpeedSensorLegacy(int pin) : pin(pin), filter(5, 5, 0.5) {}

    void setup()
    {
        pinMode(pin, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(pin), speedHandler, FALLING);
    }

    void setKalmanError(float value)
    {
        measurementError = value;
        filter.setMeasurementError(value);
    }

    void setKalmanNoise(float value)
    {
        filter.setProcessNoise(value);
    }

    float getSpeedRaw()
    {
        return rotationPerSecond;
    }

    float getSpeed()
    {
        return speed;
    }

    float getDistance()
    {
        return distance;
    }

    void loop()
    {
        volatile long localPeriod = period;
        if ((millis() - lastTimestamp) < min(localPeriod * 2, MAX_PERIOD))
            rotationPerSecond = SCALE * 1000.0 / localPeriod;
        else
            rotationPerSecond = 0;
        speed = filter.updateEstimate(rotationPerSecond);
        distance += speed;
    }

    void update()
    {
        long now = millis();
        uint16_t since = now - lastTimestamp;
        if (since > DEBOUNCE_THRESHOLD) {
            period = since;
            lastTimestamp = now;
        }
    }
};

extern SpeedSensorBase *speed_sensor_ptr;

void speedHandler()
{
    speed_sensor_ptr->update();
}
