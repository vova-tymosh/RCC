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

extern const char *locoKeys[];
extern const char *locoValues[];
extern const int locoKeySize;


class RCCLoco : public RCCNode
{
protected:
    Transport *transport;
    RccCli rccCli;

    Timer speedTimer;
    SpeedControl pid;

public:
    enum { DIR_REVERSE = 0, DIR_FORWARD = 1, DIR_STOP = 2, DIR_NEUTRAL = 3 };

    RCCLoco() : rccCli(this), speedTimer(100)
    {
        transport = new Transport(this);
        state.direction = 1;
    };

    virtual void onFunction(uint8_t code, bool activate) {}
    virtual void onThrottle(uint8_t direction, uint8_t throttle) {}
    virtual void onCommand(uint8_t code, char *value, uint8_t size) {}
    virtual void onSetValue(const char *key, const char *value) {}

    virtual int getHeartbeat()
    {
        return settings.getCachedInt("heartbeat");
    }

    int getThrottle()
    {
        return state.throttle;
    }

    void setThrottle(int value)
    {
        value = constrain(value, 0, 100);
        state.throttle = value;
        handleThrottle();
    }

    int getDirection()
    {
        return state.direction;
    }

    void setDirection(int value)
    {
        value = constrain(value, 0, 3);
        state.direction = value;
        handleThrottle();
    }

    bool getFunction(int code)
    {
        return (state.bitstate & ((uint32_t)1 << code)) != 0;
    }

    void setFunction(int code, bool activate)
    {
        if (activate)
            state.bitstate |= (uint32_t)1 << code;
        else
            state.bitstate &= ~((uint32_t)1 << code);
        onFunction(code, activate);
    }

    String getValue(const char *key)
    {
        for (int i = 0; i < sizeof(Keys) / sizeof(char *); i++) {
            if (strcmp(key, Keys[i]) == 0) {
                int value = *((uint8_t *)&state + ValueOffsets[i]);
                return String(value);
            }
        }
        String value(settings.get(key));
        return value;
    }

    void getValue(const char *key, char *value, size_t size)
    {
        for (int i = 0; i < sizeof(Keys) / sizeof(char *); i++) {
            if (strcmp(key, Keys[i]) == 0) {
                int v = *((uint8_t *)&state + ValueOffsets[i]);
                snprintf(value, size, "%d", v);
                return;
            }
        }
        settings.get(key, value, size);
    }

    void setValue(const char *key, const char *value)
    {
        settings.put(key, value);
    }

    String listValues()
    {
        String s = Keys[0];
        for (int i = 1; i < sizeof(Keys) / sizeof(char *); i++) {
            s += SEPARATOR;
            s += Keys[i];
        }
        String name = storage.openFirst();
        while (name.length() != 0) {
            s += SEPARATOR;
            s += name;
            name = storage.openNext();
        }
        return s;
    }

    void handleThrottle()
    {
        if (state.direction == DIR_STOP) {
            state.throttle = 0;
            state.throttle_out = 0;
        } else if (settings.getCachedInt("acceleration") == 0) {
            state.throttle_out = state.throttle;
        }
        onThrottle(state.direction, state.throttle_out);
    }

    void updateThrottle()
    {
        if (state.direction == DIR_STOP)
            return;
        if (settings.getCachedInt("acceleration")) {
            if (state.throttle_out < state.throttle) {
                state.throttle_out += settings.getCachedInt("acceleration");
                if (state.throttle_out > state.throttle)
                    state.throttle_out = state.throttle;
            } else if (state.throttle_out > state.throttle) {
                state.throttle_out -= settings.getCachedInt("acceleration");
                if (state.throttle_out < state.throttle)
                    state.throttle_out = state.throttle;
            }
        } else if (settings.getCachedInt("managespeed")) {
            float speed = state.speed;
            float scaled = pid.scale(speed);
            pid.setDesired(state.throttle);
            pid.setMeasured(scaled);
            state.throttle_out = pid.read();
            // Serial.println("[PD] Update: " + String(speed) + " " +
            //                String(scaled) + " " + String(state.throttle_out));
        }
        if (settings.getCachedInt("acceleration") || settings.getCachedInt("managespeed")) {
            static uint8_t lastThrottle = 0;
            if (state.throttle_out != lastThrottle) {
                lastThrottle = state.throttle_out;
                onThrottle(state.direction, state.throttle_out);
            }
        }
    }

    void begin()
    {
        char buffer[VALUE_LEN];
        settings.get("locoaddr", buffer, sizeof(buffer));
        locoAddr = atoi(buffer);
        settings.get("loconame", locoName, sizeof(locoName));
        transport->begin();
        speedTimer.start();
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
