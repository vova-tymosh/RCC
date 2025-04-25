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

    void setThrottle(int value)
    {
        Command cmd = {.code = NRF_THROTTLE, .value = (uint8_t)value};
        transport->send(&cmd);
    }

    void setDirection(int value)
    {
        Command cmd = {.code = NRF_DIRECTION, .value = (uint8_t)value};
        transport->send(&cmd);
    }

    void setFunction(int functionId, bool activate)
    {
        Command cmd = {.code = NRF_SET_FUNCTION};
        cmd.functionId = functionId;
        cmd.activate = activate;
        transport->send(&cmd);
    }

    bool getFunction(int functionId)
    {
        Command cmd = {.code = NRF_GET_FUNCTION};
        cmd.value = functionId;
        transport->send(&cmd);
        return false;
    }

    void setValue(char *key, char *value)
    {
        // Command cmd = {.code = NRF_GET_FUNCTION, .functionId = functionId};
        // send(&cmd);
    }
    

    String getValue(char *key)
    {
        return "";
    }

    void setValueLocal(char *key, char *value)
    {
        Serial.println("setValue: " + String(key) + "/" + String(value));
        settings.put(key, value);
        // for (int i = 0; i < sizeof(realtimeValue) / sizeof(realtimeValue[0]);
        //      i++) {
        //     if (strcmp(key, realtimeKey[i]) == 0) {
        //         realtimeValue[i] = atof(value);
        //         return;
        //     }
        // }
    }

    String getValueLocal(char *key)
    {
        String value(settings.get(key));
        Serial.println("getValue s: " + String(key) + "/" + String(value));
        return value;
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