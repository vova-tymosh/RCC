#include "DccPipe.h"
#include "DccPhy.h"
#include <Arduino.h>

//
// Debug/CLI interface
//
void dccPipe::processCLI()
{
    static const int INPUT_LEN_MAX = 10;
    static bool isStarted = false;
    static char inString[INPUT_LEN_MAX];
    static uint8_t inPos = 0;

    while (Serial.available()) {
        char inChar = Serial.read();
        if (inChar == '<') {
            isStarted = true;
            inPos = 0;
        } else if (inChar == '>' && isStarted) {
            inString[inPos] = '\0';
            bool result = dcc.processCommand(inString);
            Serial.print(inString);
            Serial.println(result ? " ok" : " error");
            isStarted = false;
            inString[0] = '\0';
        } else {
            inString[inPos++] = inChar;
            inPos %= INPUT_LEN_MAX;
        }
    }
}

// Speed <SDTTT>, D - forward/backward (1/0), TTT - speed (0..128)
// Function <FIIA>, II - function index, A - activate (1/0)
bool dccPipe::processCommand(char msg[])
{
    char firstChar = msg[0];
    bool isValid = false;
    switch (firstChar) {
    case 'S':
        isValid = processSpeed(msg);
        break;
    case 'F':
        isValid = processFunction(msg);
        break;
    }
    return isValid;
}

bool dccPipe::processSpeed(char msg[])
{
    if (strlen(msg) != 5) {
        return false;
    }
    char direction_text[2];
    char speed_text[4];
    strncpy(direction_text, &msg[1], 1);
    direction_text[1] = '\0';
    strncpy(speed_text, &msg[2], 3);
    speed_text[3] = '\0';
    bool direction = (bool)atoi(direction_text);
    uint8_t speed = (uint8_t)atoi(speed_text);

    cmdSpeed(direction, speed);
    return true;
}

bool dccPipe::processFunction(char msg[])
{
    if (strlen(msg) != 4) {
        return false;
    }
    char function_text[3];
    char activate_text[2];
    strncpy(function_text, &msg[1], 2);
    function_text[2] = '\0';
    strncpy(activate_text, &msg[3], 1);
    activate_text[1] = '\0';
    uint8_t function = (uint8_t)atoi(function_text);
    bool activate = (bool)atoi(activate_text);

    cmdFunction(function, activate);
    return true;
}

//
// Execution part
//

dccPipe::dccPipe()
{
    this->reset_pkt = dccProtocol::getResetPacket();
    this->control_state = dccControllerState::Idle;
    this->function_bits = 0;
}

void dccPipe::setup()
{
    dccPhy::setup();
    Serial.println("DCC_begin");
    if (control_state == Idle) {
#ifndef ARDUINO_ARCH_NRF52
        cli();
#endif
        control_state = Sending;
#ifndef ARDUINO_ARCH_NRF52
        sei();
#endif
    }
}

void dccPipe::cmdSpeed(uint8_t direction, uint8_t speed)
{
    this->packet =
        dccProtocol::getSpeedPacket(DEFAULT_ADDRESS, direction, speed);
}

void dccPipe::cmdFunction(uint8_t function, bool activate)
{
    if (activate) {
        function_bits |= (uint32_t)1 << function;
    } else {
        function_bits &= ~((uint32_t)1 << function);
    }

    if (function <= 4) {
        this->packet = dccProtocol::getFunctionPacket(
            DEFAULT_ADDRESS, function_bits, Packet::Type::FUNCTION_04);
    } else if (function <= 8) {
        this->packet = dccProtocol::getFunctionPacket(
            DEFAULT_ADDRESS, function_bits, Packet::Type::FUNCTION_58);
    } else if (function <= 12) {
        this->packet = dccProtocol::getFunctionPacket(
            DEFAULT_ADDRESS, function_bits, Packet::Type::FUNCTION_912);
    } else if (function <= 20) {
        this->packet = dccProtocol::getFunctionPacket(
            DEFAULT_ADDRESS, function_bits, Packet::Type::FUNCTION_1320);
    } else if (function < 28) {
        this->packet = dccProtocol::getFunctionPacket(
            DEFAULT_ADDRESS, function_bits, Packet::Type::FUNCTION_2128);
    }
}

Packet dccPipe::getPacket()
{
    if (this->control_state == Sending) {
        return this->packet;
    } else {
        return this->reset_pkt;
    }
}

dccPipe dcc;
