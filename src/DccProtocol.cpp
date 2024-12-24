#include "DccProtocol.h"
#include <Arduino.h>

// The definition of DCC protocol is based on NMRA S-9.2.1:
//  https://www.nmra.org/sites/default/files/s-9.2.1_2012_07.pdf

struct Function {
    uint8_t prefix;
    uint8_t offset;
    uint8_t n;
    Packet::Type type;
};

static constexpr struct Function functions[] = {
    {0x3F, 0, 4, Packet::Type::SPEED_128},
    {0x80, 1, 4, Packet::Type::FUNCTION_04},
    {0xB0, 5, 4, Packet::Type::FUNCTION_58},
    {0xA0, 9, 4, Packet::Type::FUNCTION_912},
    {0xDE, 13, 8, Packet::Type::FUNCTION_1320},
    {0xDF, 21, 8, Packet::Type::FUNCTION_2128},
};

Packet dccProtocol::getSpeedPacket(uint8_t address, uint8_t direction,
                                   uint8_t speed)
{
    // The format of this instruction is 001CCCCC 0 GDDDDDDD
    // CCCCC = 11111: 128 Speed Step Control
    // G = "1" is forward and "0" is reverse
    // DDDDDDD = 0000000 for stop, 0000001 for emergency stop,
    //     the rest is 126 steps of speed

    Packet m;
    const Function *func = &functions[0];
    m.len = 4;

    m.data[0] = address;
    m.data[1] = func->prefix;
    m.data[2] = ((direction) ? 0x80 : 0x00) | speed;
    m.data[3] = (m.data[0] ^ m.data[1]) ^ m.data[2];

    m.type = func->type;
    return m;
};

Packet dccProtocol::getFunctionPacket(uint8_t address, uint32_t function_bits,
                                      uint8_t function_type)
{
    // Function instructions. DCC defines 5 regions for the functions with
    // messages of 3-4 bytes. Specific message type, length and other details
    // are stored in _functions_ array.

    Packet m;
    if (function_type > Packet::Type::FUNCTION_2128) {
        return m;
    }
    const Function *func = &functions[function_type];

    m.len = (func->type >= Packet::Type::FUNCTION_1320) ? 4 : 3;
    m.data[0] = address;
    m.data[1] = func->prefix;

    uint8_t data = 0x00;
    for (uint8_t i = 0; i < func->n; i++) {
        data |= (function_bits & ((uint32_t)1 << (func->offset + i))) >>
                func->offset;
    }
    if (func->type == Packet::Type::FUNCTION_04) {
        data |= (function_bits & 1) << 4;
    }

    if (m.len == 3) {
        m.data[1] |= data;
        m.data[2] = m.data[0] ^ m.data[1];
    } else {
        m.data[2] = data;
        m.data[3] = (m.data[0] ^ m.data[1]) ^ m.data[2];
    }

    Serial.println("F:" + String(m.len) + "," + String(m.data[0], HEX) + "," +
                   String(m.data[1], HEX) + "," + String(m.data[2], HEX) + "," +
                   String(m.data[3], HEX));

    m.type = static_cast<Packet::Type>(function_type);
    return m;
}

Packet dccProtocol::getResetPacket()
{
    // Reset Packet For All Decoders
    // erase all volatile memory (including any speed and direction data)
    // bring the locomotive to an immediate stop
    Packet m;
    m.len = 3;
    m.data[0] = 0x00;
    m.data[1] = 0x00;
    m.data[2] = 0x00;
    return m;
}

Packet dccProtocol::getIdlePacket()
{
    // Idle Packet For All Decoders
    Packet m;
    m.len = 3;
    m.data[0] = 0xFF;
    m.data[1] = 0x00;
    m.data[2] = 0xFF;
    return m;
}

Packet dccProtocol::getBroadcastStopPacket()
{
    // Broadcast Stop Packets For All Decoders
    Packet m;
    m.len = 3;
    m.data[0] = 0x00;
    m.data[1] = 0x51;
    m.data[2] = 0x51;
    return m;
}
