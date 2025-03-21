/*
 * Ruby Conn-ectivity
 *
 *  Timer
 */
#pragma once
#include <Arduino.h>

class Timer
{
private:
    unsigned long fire = 0;
    unsigned int duration = 0;

public:
    Timer() {}

    Timer(unsigned int duration)
    {
        start(duration);
    }

    void start()
    {
        start(this->duration);
    }

    void start(unsigned int duration)
    {
        this->duration = duration;
        this->fire = millis() + duration;
    }

    bool hasFiredOnce()
    {
        if ((fire != 0) && (millis() > fire)) {
            fire = 0;
            return true;
        } else {
            return false;
        }
    }

    bool hasFired()
    {
        if (hasFiredOnce()) {
            start();
            return true;
        } else {
            return false;
        }
    }
};
