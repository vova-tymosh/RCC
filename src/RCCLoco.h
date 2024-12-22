/*
 * Railroad communication
 *
 *
 */
#pragma once
#include "RCCState.h"
#include "Transport.h"
// #include "SpeedControl.h"
#include "Storage.h"
#include "Timer.h"

class RCCLoco : public RCCLocoBase
{
protected:
    Transport *transport;
    Storage *storage;
    Timer timer;
    // SpeedControl pid;
    int increment;

public:
    LocoState state;

    RCCLoco(Storage *storage = NULL) : storage(storage), increment(1)
    {
        transport = new Transport(this);
        state.direction = 1;
    };

    virtual void onFunction(bool activate, uint8_t code) {}
    virtual void onThrottle(uint8_t direction, uint8_t throttle) {}
    virtual void onCommand(uint8_t code, float value) {}

    void setFunction(bool activate, int code)
    {
        if (activate)
            state.bitstate |= (uint32_t)1 << code;
        else
            state.bitstate &= ~((uint32_t)1 << code);
        if (storage)
            storage->save(state.bitstate);
        onFunction(activate, code);
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
        //TODO modify TransportNRF to use 0/1 direction only
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
                // Serial.println("PID: " + String(speed) + " " + String(scaled) +
                //                " " + String(state.throttle_out));
            } else {
                state.throttle_out = throttle;
            }
        }
        log("handleThrottle");
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
        if (storage) {
            state.bitstate = storage->restore(0);
            p = fromBinary(storage->restore(1));
            i = fromBinary(storage->restore(2));
            d = fromBinary(storage->restore(3));
            m = fromBinary(storage->restore(4));
            //TODO: restore all the functions
        }
        // pid.setup(p, i, d, m);
        timer.restart();
    }

    void loop()
    {
        transport->loop();
    }
};
