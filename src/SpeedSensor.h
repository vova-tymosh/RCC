/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
/*
 * Speed Sensor (rotation counter)
 *
 *
 *
 */
#include "Timer.h"

void speedHandler();

class SpeedSensor
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
          update_period(update_period)
    {
    }

    void begin()
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

extern SpeedSensor speedSensor;

void speedHandler()
{
    speedSensor.update();
}
