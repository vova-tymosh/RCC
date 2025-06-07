/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
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
        this->duration = duration;
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
