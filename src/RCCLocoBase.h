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

    virtual void onFunction(uint8_t code, bool activate) {}
    virtual void onThrottle(uint8_t direction, uint8_t throttle) {}
    virtual void onCommand(uint8_t code, float value) {}
    virtual void setFunction(int code, bool activate)  {}
    virtual bool getFunction(int code) {return false;}
    virtual void setThrottle(int value) {}
    virtual int getThrottle() {return 0;}
    virtual void setDirection(int value, bool stopState = false) {}
    virtual int getDirection() {return 0;}
    virtual void putValue(char *key, char *value) {}
    virtual String getValue(char *key) {return "";}
    virtual String listValues() {return "";}
    virtual void setup() {}
    virtual void loop() {}
};
