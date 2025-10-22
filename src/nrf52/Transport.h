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
#define log(msg)                                                                                   \
    {                                                                                              \
        Serial.print(msg);                                                                         \
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

    void heartbeatKeys()
    {
        char packet[MAX_PACKET];
        char *p = packet;
        p = strcatm(p, NRF_HEARTBEAT_KEYS);
        for (int i = 0; i < sizeofarray(Keys) - 1; i++) {
            p = strcatm(p, Keys[i]);
            p = strcatm(p, NRF_SEPARATOR);
        }
        p = strcatm(p, Keys[sizeofarray(Keys) - 1]);
        write(packet, strlen(packet));
    }

    void introduce()
    {
        char packet[MAX_PACKET];
        char *p = packet;
        p = strcatm(p, NRF_INTRO);
        p = strcatm(p, NRF_TYPE_LOCO);
        p = strcatm(p, NRF_SEPARATOR);
        p = strcatm(p, node->locoAddr);
        p = strcatm(p, NRF_SEPARATOR);
        p = strcatm(p, node->locoName);
        p = strcatm(p, NRF_SEPARATOR);
        p = strcatm(p, VERSION);
        p = strcatm(p, NRF_SEPARATOR);
        p = strcatm(p, LOCO_FORMAT);
        write(packet, strlen(packet));
    }

    void processList()
    {
        String list = node->listValues();
        int size = list.length();
        if (size > MAX_PACKET - 1) {
            int lastSeparator = list.lastIndexOf(NRF_SEPARATOR, MAX_PACKET);
            String packet1 = String(NRF_LIST_VALUE_RES) + list.substring(0, lastSeparator);
            write(packet1.c_str(), packet1.length());
            String packet2 = String(NRF_LIST_VALUE_RES) + list.substring(lastSeparator + 1, size);
            write(packet2.c_str(), packet2.length());
        } else {
            String packet = String(NRF_LIST_VALUE_RES) + list;
            write(packet.c_str(), size);
        }
    }

    void heartbeat()
    {
        node->state.packet_type = NRF_HEARTBEAT;
        node->state.tick = (float)millis() / 100;
        write(&node->state, sizeof(node->state));
    }

    void write(const void *payload, uint16_t size)
    {
        if (size > 0) {
            uint8_t *p = (uint8_t *)payload;
            log("[Nrf] > ");
            log((char)p[0]);
            log(" ");
            log(size);
            log("\n");
            wireless.write(payload, size);
        }
    }

    void received(uint8_t *payload, uint16_t size)
    {
        if (size < COMMAND_SIZE)
            return;
        struct Command *command = (struct Command *)payload;
        log("[Nrf] < ");
        log((char)command->code);
        log(" ");
        log(command->value);
        log("\n");

        if (command->code == NRF_INTRO) {
            introduce();
            heartbeatKeys();
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
            write(&reply, sizeof(reply));
        } else if (command->code == NRF_SET_VALUE) {
            if (size >= CODE_SIZE + 1) {
                payload[size] = 0;
                char *buffer[2];
                int tokens = split((char *)payload + CODE_SIZE, (char **)&buffer,
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
                char reply[MAX_PACKET];
                memcpy(reply, payload, size);
                reply[0] = NRF_SET_VALUE;
                reply[size] = NRF_SEPARATOR;
                size++;
                node->getValue(key, reply + size, MAX_PACKET - size);
                write(reply, strlen(reply));
            }
        } else if (command->code == NRF_LIST_VALUE_REQ) {
            processList();
        } else if (command->code == NRF_HEARTBEAT) {
            heartbeat();
        } else {
            node->onCommand(command->code, (char *)payload, size);
        }
    }

    void begin()
    {
        wireless.begin(node->locoAddr);
        introduce();
        heartbeatKeys();
        heartbeatTimer.start();
        node->onConnect(CONN_NRF);
    }

    void loop()
    {
        if (wireless.available()) {
            uint16_t size = wireless.read(payload, sizeof(payload) - 1);
            received(payload, size);
        }

        int hb = node->getHeartbeat();
        if (hb && heartbeatTimer.hasFiredOnce()) {
            heartbeat();
            heartbeatTimer.start(hb);
        }
    }
};
