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
#include "Settings.h"

void speedHandler();

class SpeedSensor
{
private:
    const int update_period;
    const int pin;
    float distance_per_click;
    Timer timer;
    float distance;
    float last_disatnce;
    unsigned long last_time;
    float speed;

public:
    SpeedSensor(int pin, int update_period = 500)
        : pin(pin), update_period(update_period)
    {
    }

    void begin()
    {
        char buffer[VALUE_LEN];
        settings.get("distancePerClick", buffer, sizeof(buffer));
        distance_per_click = atof(buffer);
        if (distance_per_click == 0)
            distance_per_click = 1;
        pinMode(pin, INPUT);
        attachInterrupt(digitalPinToInterrupt(pin), speedHandler, RISING);
        last_time = millis();
        timer.start(update_period);
    }

    void loop()
    {
        if (timer.hasFired()) {
            unsigned long now = millis();
            speed = (float)(distance - last_disatnce) / ((float)(now - last_time) / 1000);
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
