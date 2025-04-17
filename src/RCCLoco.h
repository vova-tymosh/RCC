/*
 * Railroad communication
 *
 *
 */
#pragma once
#include "RCCState.h"
#include "SpeedControl.h"
#include "Settings.h"
#include "Transport.h"
#include "Timer.h"
#include "Cli.h"


class RCCLoco : public RCCNode
{
protected:
    Transport *transport;
    RccCli rccCli;

    Timer speedTimer;
    SpeedControl pid;
    bool stopState = false;

    enum {
        ACCELERATION = 0,
        MANAGESPEED,
        MAX_REALTIME,
    };
    const char* realtimeKey[MAX_REALTIME] = {
        "acceleration", "managespeed",
    };
    float realtimeValue[MAX_REALTIME] = {
        0, 0,
    };

public:
    RCCLoco() : rccCli(this), speedTimer(100) 
    {
        transport = new Transport(this);
        state.direction = 1;
    };

    virtual void onFunction(uint8_t code, bool activate) {}
    virtual void onThrottle(uint8_t direction, uint8_t throttle) {}
    virtual void onCommand(uint8_t code, float value) {}

    void setFunction(int code, bool activate)
    {
        if (activate)
            state.bitstate |= (uint32_t)1 << code;
        else
            state.bitstate &= ~((uint32_t)1 << code);
        onFunction(code, activate);
    }

    bool getFunction(int code)
    {
        return (state.bitstate & ((uint32_t)1 << code)) != 0;
    }

    void setThrottle(int value)
    {
        value = constrain(value, 0, 100);
        state.throttle = value;
        handleThrottle();
    }

    int getThrottle()
    {
        return state.throttle;
    }

    void setDirection(int value, bool _stopState = false)
    {
        value = constrain(value, 0, 3);
        state.direction = value;
        stopState = _stopState;
        handleThrottle();
    }

    int getDirection()
    {
        return state.direction;
    }    

    void putValue(char *key, char *value)
    {
        Serial.println("putValue: " + String(key) + "/" + String(value));
        settings.put(key, value);
        for (int i = 0; i < sizeof(realtimeValue)/sizeof(realtimeValue[0]); i++) {
            if (strcmp(key, realtimeKey[i]) == 0) {
                realtimeValue[i] = atof(value);
                return;
            }
        }
    }

    String getValue(char *key)
    {
        for (int i = 0; i < sizeof(Keys) / sizeof(char *); i++) {
            if (strcmp(key, Keys[i]) == 0) {
                int value = *((uint8_t *)&state + ValueOffsets[i]);
                Serial.println("getValue r: " + String(key) + "/" + String(value));
                return String(value);
            }
        }
        String value(settings.get(key));
        Serial.println("getValue s: " + String(key) + "/" + String(value));
        return value;
    }

    String listValues()
    {
        String s = Keys[0];
        for (int i = 1; i < sizeof(Keys) / sizeof(char *); i++) {
            s += " ";
            s += Keys[i];
        }
        for (int i = 0; i < settingsSize; i++) {
            s += " ";
            s += settingsKeys[i];
        }
        return s;
    }

    void handleThrottle()
    {
        if (stopState) {
            state.throttle = 0;
            state.throttle_out = 0;
        } else if (realtimeValue[ACCELERATION] == 0) {
            state.throttle_out = state.throttle;
        }
        onThrottle(state.direction, state.throttle_out);
    }

    void updateThrottle()
    {
        if (realtimeValue[ACCELERATION]) {
            if (state.throttle_out < state.throttle) {
                state.throttle_out += realtimeValue[ACCELERATION];
                if (state.throttle_out > state.throttle)
                    state.throttle_out = state.throttle;
            }
            else if (state.throttle_out > state.throttle) {
                state.throttle_out -= realtimeValue[ACCELERATION];
                if (state.throttle_out < state.throttle)
                    state.throttle_out = state.throttle;
            }
        } else if (realtimeValue[MANAGESPEED]) {
            float speed = state.speed;
            float scaled = pid.scale(speed);
            pid.setDesired(state.throttle);
            pid.setMeasured(scaled);
            state.throttle_out = pid.read();
            Serial.println("[PD] Update: " + String(speed) + " " + String(scaled) + " " + String(state.throttle_out));
        }
        if (realtimeValue[ACCELERATION] || realtimeValue[MANAGESPEED]) {
            static uint8_t lastThrottle = 0;
            if (state.throttle_out != lastThrottle) {
                lastThrottle = state.throttle_out;
                onThrottle(state.direction, state.throttle_out);
            }
        }
    }

    void setup()
    {
        locoName = settings.get("loconame");
        locoAddr = settings.get("locoaddr");
        for (int i = 0; i < sizeof(realtimeValue)/sizeof(realtimeValue[0]); i++) {
            realtimeValue[i] = settings.get(realtimeKey[i]).toFloat();
        }
        transport->begin();
    }

    void loop()
    {
        transport->loop();
        rccCli.loop();
        if (speedTimer.hasFired()) {
            updateThrottle();
        }
    }
};
