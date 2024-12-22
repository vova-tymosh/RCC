/*
 * Railroad communication
 *
 *
 */
#pragma once
#include "RCCState.h"

class RCCLocoBase
{
public:
    LocoState state;

    int debugLevel;

    void log(String msg)
    {
        if (debugLevel > 0)
            Serial.println(msg);
    }

    virtual void onFunction(uint8_t code, bool activate) = 0;
    virtual void onThrottle(uint8_t direction, uint8_t throttle) = 0;
    virtual void onCommand(uint8_t code, float value) = 0;
    virtual void setFunction(int code, bool activate) = 0;
    virtual bool getFunction(int code) = 0;
    virtual void setThrottle(int value) = 0;
    virtual void setDirection(int value) = 0;
    virtual int getThrottle() = 0;
    virtual int getDirection() = 0;
    virtual void setup() = 0;
    virtual void loop() = 0;
};
