/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
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
