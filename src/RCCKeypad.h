#pragma once

#if !defined(ARDUINO_ARCH_NRF52)
#error Architecture/Platform is not supported!
#endif

#include "RCCState.h"
#include "Settings.h"
#include "nrf52/KeypadTransport.h"
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
    };

    void setThrottle(int value)
    {
        transport->writeThrottle(value);
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