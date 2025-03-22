/*
 * Railroad communication
 *
 *
 */
#pragma once
#include "RCCState.h"
#include "Transport.h"
#include "Cli.h"
// #include "SpeedControl.h"
#include "Storage.h"
#include "Timer.h"

// fix PID, Storage

class RCCLoco : public RCCLocoBase
{
protected:
    Transport *transport;
    RccCli rccCli;
    Timer timer;
    // SpeedControl pid;
    int increment;

public:
    RCCLoco() : increment(1), rccCli(this)
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
        // storage->write("bitstate", &state.bitstate, sizeof(state.bitstate));
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
        if ((state.slow == false) && (state.pid == false))
            onThrottle(state.direction, state.throttle);
    }

    void setDirection(int value)
    {
        value = constrain((int)value, 0, 1);
        state.direction = value;
        if ((state.slow == false) && (state.pid == false))
            onThrottle(state.direction, state.throttle);
    }

    int getThrottle()
    {
        return state.throttle;
    }

    int getDirection()
    {
        return state.direction;
    }

    void putValue(char *key, char *value)
    {
        // for(int i = 0; i < sizeof(Keys)/sizeof(char*); i++) {
        //     if (strcmp(key, Keys[i]) == 0) {
        //         *((uint8_t*)&state + ValueOffsets[i]) = atoi(value);
        //         return;
        //     }
        // }
        settings.put(key, value);
    }

    String getValue(char *key)
    {
        for(int i = 0; i < sizeof(Keys)/sizeof(char*); i++) {
            if (strcmp(key, Keys[i]) == 0) {
                int value = *((uint8_t*)&state + ValueOffsets[i]);
                return String(value);
            }
        }
        return settings.get(key);
    }

    String listValues()
    {
        String s = FIELDS;
        for(int i = 0; i < settingsSize; i++) {
            s += " ";
            s += settingsKeys[i];
        }
        return s;
    }

    void handleThrottle()
    {
        uint8_t throttle;
        if (state.direction == 0) {
            state.throttle = 0;
            state.throttle_out = 0;
            throttle = 0;
        } else {
            if (state.slow) {
                static uint8_t slow_throttle = 0;
                if (slow_throttle < state.throttle)
                    slow_throttle += increment;
                else if (slow_throttle > state.throttle)
                    slow_throttle -= increment;
                throttle = slow_throttle;
            } else {
                throttle = state.throttle;
            }
            if (state.pid) {
                // float speed = state.speed;
                // float scaled = pid.scale(speed);
                // pid.setDesired(throttle);
                // pid.setMeasured(scaled);
                // state.throttle_out = pid.read();
                // Serial.println("PID: " + String(speed) + " " + String(scaled)
                // +
                //                " " + String(state.throttle_out));
            } else {
                state.throttle_out = throttle;
            }
        }
        onThrottle(state.direction, state.throttle_out);
    }

    float fromBinary(uint32_t x)
    {
        union {
            float f;
            uint32_t i;
        } t;
        t.i = x;
        return t.f;
    }

    uint32_t toBinary(float x)
    {
        union {
            float f;
            uint32_t i;
        } t;
        t.f = x;
        return t.i;
    }

    void setup()
    {
        transport->begin();
        float p = 0;
        float i = 0;
        float d = 0;
        float m = 0;
        // if (storage) {
        // storage->read("bitstate", &state.bitstate, sizeof(state.bitstate));

            // p = fromBinary(storage->restore(1));
            // i = fromBinary(storage->restore(2));
            // d = fromBinary(storage->restore(3));
            // m = fromBinary(storage->restore(4));
            // TODO: restore all the functions
        // }
        // pid.setup(p, i, d, m);
        // timer.start();
    }

    void loop()
    {
        transport->loop();
        rccCli.loop();
    }
};
