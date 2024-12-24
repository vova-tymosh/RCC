#pragma once
#include <stdint.h>

#define DEFAULT_ADDRESS 3

struct Packet {
    enum Type {
        SPEED_128,   // Advanced Operations Instruction - 128 Speed Step Control
        FUNCTION_04, // Function Group One Instruction - FL and F1-F4 Function
                     // Control
        FUNCTION_58, // Function Group Two Instruction - F5-F8 Function Control
        FUNCTION_912,  // Function Group Two Instruction - F9-F12 Function
                       // Control
        FUNCTION_1320, // Feature Expansion Instruction - F13-F20 Function
                       // Control
        FUNCTION_2128, // Feature Expansion Instruction - F21-F28 Function
                       // Control
        RESET,
        IDLE
    };
    uint8_t data[6];
    uint8_t len;
    Type type;
};

class dccProtocol
{
public:
    static Packet getSpeedPacket(uint8_t address, uint8_t direction,
                                 uint8_t speed);
    static Packet getFunctionPacket(uint8_t address, uint32_t function_bits,
                                    uint8_t function_type);
    static Packet getResetPacket();
    static Packet getIdlePacket();
    static Packet getBroadcastStopPacket();
};
