#pragma once
#include <stdint.h>
#include "DccProtocol.h"

typedef enum {
    Idle,
    Sending,
} dccControllerState;

class dccPipe
{
private:
    dccControllerState control_state;
    uint32_t function_bits;
    Packet reset_pkt;
    Packet packet;

public:
    dccPipe();
    void begin();
    bool processCommand(char msg[]);
    bool processSpeed(char msg[]);
    bool processFunction(char msg[]);
    void processCLI();

    void cmdSpeed(uint8_t direction, uint8_t speed);
    void cmdFunction(uint8_t function, bool activate);

    Packet getPacket();
};

extern dccPipe dcc;
