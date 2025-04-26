#pragma once

typedef enum { Preamble, Seperator, SendByte } dccPulseState;

class dccPhy
{
private:
    static void writeHigh();
    static void writeLow();

public:
    static void begin();
    static void handleInterrupt();
};
