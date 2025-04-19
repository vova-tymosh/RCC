#pragma once

#if !defined(ARDUINO_ARCH_NRF52)
#error Architecture/Platform is not supported!
#endif

#include "RCCState.h"
#include "Settings.h"
#include "nrf52/KeypadTransport.h"
#include "nrf52/Protocol.h"
#include "Timer.h"
#include "Cli.h"

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

    void setDirection(int value, bool stopState = false)
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

    // void putValue(char *key, char *value)
    // {
    //     Command cmd = {.code = NRF_GET_FUNCTION, .functionId = functionId};
    //     send(&cmd);
    // }
    

    // String getValue(char *key)
    // {
    //     return "";
    // }
    String listValues()
    {
        Command cmd = {.code = NRF_LIST_VALUE, .value = 0};
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