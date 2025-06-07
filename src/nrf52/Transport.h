/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
/*
 * Railroad communication
 *
 *
 */
#pragma once
#include "nrf52/Wireless.h"
#include "nrf52/TransportUtils.h"
#include "Protocol.h"
#include "RCCNode.h"
#include "Timer.h"

#if RCC_DEBUG >= 2
#define log(msg)                                                               \
    {                                                                          \
        Serial.println(String("[Nrf] ") + (msg));                              \
    };
#else
#define log(msg)
#endif

class Transport
{
private:
    Timer heartbeatTimer;
    Wireless wireless;
    RCCNode *node;
    uint8_t payload[MAX_PACKET];

public:
    Transport(RCCNode *node, int heartbeatPeriod = 1000)
        : node(node), heartbeatTimer(heartbeatPeriod) {};

    void heartBeatKeys()
    {
        String packet = String(NRF_HEARTBEAT_KEYS);
        packet.reserve(256);
        for (int i = 0; i < sizeofarray(Keys) - 1; i++) {
            packet += Keys[i];
            packet += NRF_SEPARATOR;
        }
        packet += Keys[sizeofarray(Keys) - 1];
        int size = packet.length();
        wireless.write(packet.c_str(), size);
    }

    void introduce()
    {
        String packet = String(NRF_INTRO) + NRF_TYPE_LOCO + NRF_SEPARATOR +
                        node->locoAddr + NRF_SEPARATOR + node->locoName +
                        NRF_SEPARATOR + VERSION + NRF_SEPARATOR + LOCO_FORMAT;
        int size = packet.length();
        wireless.write(packet.c_str(), size);
        heartBeatKeys();
    }

    void processList()
    {
        String list = node->listValues();
        int size = list.length();
        if (size > MAX_PACKET - 1) {
            int lastSeparator = list.lastIndexOf(NRF_SEPARATOR, MAX_PACKET);
            String packet1 =
                String(NRF_LIST_VALUE_RES) + list.substring(0, lastSeparator);
            wireless.write(packet1.c_str(), packet1.length());
            String packet2 = String(NRF_LIST_VALUE_RES) +
                             list.substring(lastSeparator + 1, size);
            wireless.write(packet2.c_str(), packet2.length());
        } else {
            String packet = String(NRF_LIST_VALUE_RES) + list;
            wireless.write(packet.c_str(), size);
        }
    }

    void heartbeat()
    {
        node->state.packet_type = NRF_HEARTBEAT;
        node->state.tick = (float)millis() / 100;
        wireless.write(&node->state, sizeof(node->state));
    }

    void received(uint8_t *payload, uint16_t size)
    {
        if (size < COMMAND_SIZE)
            return;
        struct Command *command = (struct Command *)payload;
        log("<" + String((char)command->code) + " " + String(command->value));

        if (command->code == NRF_INTRO) {
            introduce();
        } else if (command->code == NRF_THROTTLE) {
            node->setThrottle(command->value);
        } else if (command->code == NRF_DIRECTION) {
            node->setDirection(command->value);
        } else if (command->code == NRF_SET_FUNCTION) {
            node->setFunction(command->functionId, command->activate);
        } else if (command->code == NRF_GET_FUNCTION) {
            Command reply;
            reply.code = NRF_SET_FUNCTION;
            reply.functionId = command->functionId;
            reply.activate = node->getFunction(command->functionId);
            wireless.write(&reply, sizeof(reply));
        } else if (command->code == NRF_SET_VALUE) {
            if (size >= CODE_SIZE + 1) {
                payload[size] = 0;
                char *buffer[2];
                int tokens =
                    split((char *)payload + CODE_SIZE, (char **)&buffer,
                          sizeofarray(buffer), NRF_SEPARATOR);
                if (tokens >= 2) {
                    char *key = buffer[0];
                    char *value = buffer[1];
                    node->setValue(key, value);
                }
            }
        } else if (command->code == NRF_GET_VALUE) {
            if (size >= CODE_SIZE + 1) {
                payload[size] = 0;
                char *key = (char *)(payload + CODE_SIZE);
                String value = node->getValue(key);
                char reply[MAX_PACKET];
                memcpy(reply, payload, size);
                reply[size] = NRF_SEPARATOR;
                size++;
                memcpy(reply + size, value.c_str(), value.length());
                reply[0] = NRF_SET_VALUE;
                size += value.length();
                if (size > MAX_PACKET)
                    size = MAX_PACKET;
                wireless.write(reply, size);
            }
        } else if (command->code == NRF_LIST_VALUE_ASK) {
            processList();
        } else if (command->code == NRF_HEARTBEAT) {
            heartbeat();
        } else {
            node->onCommand(command->code, (char *)payload + CODE_SIZE, size);
        }
    }

    void begin()
    {
        wireless.begin(node->locoAddr);
        introduce();
        heartbeatTimer.start();
    }

    void loop()
    {
        if (wireless.available()) {
            uint16_t size = wireless.read(payload, sizeof(payload) - 1);
            received(payload, size);
        }

        if (heartbeatTimer.hasFiredOnce()) {
            heartbeat();
            heartbeatTimer.start(node->getHeartbeat());
        }
    }
};
