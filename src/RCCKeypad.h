#pragma once

#if !defined(ARDUINO_ARCH_NRF52)
#error Architecture/Platform is not supported!
#endif

#include "RCCState.h"
#include "Settings.h"
#include "nrf52/KeypadTransport.h"
#include "Protocol.h"
#include "Timer.h"
#include "Cli.h"

extern const char *keypadKeys[];
extern const char *keypadValues[];
extern const int keypadKeySize;


class RCCKeypad : public RCCNode
{
protected:
    KeypadTransport *transport;
    RccCli rccCli;

public:
    RCCKeypad() : rccCli(this)
    {
        transport = new KeypadTransport(this);
    }

    virtual void onFunction(uint8_t code, bool activate) {}
    virtual void onThrottle(uint8_t direction, uint8_t throttle) {}
    virtual void onCommand(uint8_t code, char* value, uint8_t size) {}

    int getThrottle()
    {
        return 0;
    }

    void setThrottle(int value)
    {
        Command cmd = {.code = NRF_THROTTLE, .value = (uint8_t)value};
        transport->send(&cmd);
    }

    int getDirection()
    {
        return 0;
    }

    void setDirection(int value)
    {
        Command cmd = {.code = NRF_DIRECTION, .value = (uint8_t)value};
        transport->send(&cmd);
    }

    bool getFunction(int functionId)
    {
        Command cmd = {.code = NRF_GET_FUNCTION};
        cmd.value = functionId;
        transport->send(&cmd);
        return false;
    }

    void setFunction(int functionId, bool activate)
    {
        Command cmd = {.code = NRF_SET_FUNCTION};
        cmd.functionId = functionId;
        cmd.activate = activate;
        transport->send(&cmd);
    }
    
    String getValue(char *key)
    {
        String packet = String(NRF_GET_VALUE) + key;
        int size = packet.length();
        transport->send((uint8_t *)packet.c_str(), size);   
        return "";
    }

    void setValue(char *key, char *value)
    {
        String packet = String(NRF_SET_VALUE) + key + NRF_SEPARATOR + value;
        int size = packet.length();
        transport->send((uint8_t *)packet.c_str(), size);   
    }

    String getValueLocal(char *key)
    {
        String value(settings.get(key));
        Serial.println("getValueLoc: " + String(key) + "/" + String(value));
        return value;
    }

    void setValueLocal(char *key, char *value)
    {
        Serial.println("setValueLoc: " + String(key) + "/" + String(value));
        settings.put(key, value);
        // for (int i = 0; i < sizeof(realtimeValue) / sizeof(realtimeValue[0]);
        //      i++) {
        //     if (strcmp(key, realtimeKey[i]) == 0) {
        //         realtimeValue[i] = atof(value);
        //         return;
        //     }
        // }
    }

    String listValues()
    {
        Command cmd = {.code = NRF_LIST_VALUE_ASK, .value = 0};
        transport->send(&cmd);
        return "";
    }

    void setup()
    {
        locoAddr = settings.get("locoaddr");
        transport->setup();
    }

    void loop()
    {
        transport->loop();
        rccCli.loop();
    }
};