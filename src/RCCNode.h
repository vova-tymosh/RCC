/*
 * Railroad communication
 *
 *
 */
#pragma once
#include "RCCState.h"

class RCCNode
{
public:
    LocoState state;
    String locoName;
    String locoAddr;

    int debugLevel;

    void log(String msg)
    {
        if (debugLevel > 0)
            Serial.println(msg);
    }

    virtual int getHeartbeat()
    {
        return 1000;
    }

    virtual void onFunction(uint8_t code, bool activate) = 0;
    virtual void onThrottle(uint8_t direction, uint8_t throttle) = 0;
    virtual void onCommand(uint8_t code, char* value, uint8_t size) = 0;
    virtual void setFunction(int code, bool activate) = 0;
    virtual bool getFunction(int code) = 0;
    virtual void setThrottle(int value) = 0;
    virtual int getThrottle() = 0;
    virtual void setDirection(int value) = 0;
    virtual int getDirection() = 0;
    virtual void setValue(char *key, char *value) = 0;
    virtual String getValue(char *key) = 0;
    virtual String listValues() = 0;
    virtual void setup() = 0;
    virtual void loop() = 0;
};
