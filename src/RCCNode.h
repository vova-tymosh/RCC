/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
/*
 * Railroad communication
 *
 *
 */
#pragma once
#include "RCCState.h"
#include "Platform.h"

class RCCNode
{
public:
    LocoState state;
    char locoName[VALUE_LEN];
    int locoAddr;

    virtual int getHeartbeat()
    {
        return 1000;
    }

    virtual void onFunction(uint8_t code, bool activate) = 0;
    virtual void onThrottle(uint8_t direction, uint8_t throttle) = 0;
    virtual void onCommand(uint8_t code, char *value, uint8_t size) = 0;
    virtual void onSetValue(const char *key, const char *value) = 0;
    virtual void onConnect(uint8_t connType) = 0;
    virtual void setFunction(int code, bool activate) = 0;
    virtual bool getFunction(int code) = 0;
    virtual void setThrottle(int value) = 0;
    virtual int getThrottle() = 0;
    virtual void setDirection(int value) = 0;
    virtual int getDirection() = 0;
    virtual void setValue(const char *key, const char *value) = 0;
    virtual String getValue(const char *key) = 0;
    virtual void getValue(const char *key, char *value, size_t size) = 0;
    virtual String listValues() = 0;
    virtual void begin() = 0;
    virtual void loop() = 0;
};
