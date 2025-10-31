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

#define MAX_LOCO 5
#define NAME_SIZE 4

#if RCC_DEBUG >= 2
#define log(msg)                                                                                   \
    {                                                                                              \
        Serial.println(String("[Nrf] ") + (msg));                                                  \
    };
#else
#define log(msg)
#endif

struct Qos {
    int sendExp = 0;
    int sendAct = 0;
    int heartbeatAct = 0;
    int heartbeatExp = 0;
    int heartbeatPeriod = 0;
    int rate = 0;
    int span = 0;
    Timer timer;

    void begin()
    {
        heartbeatPeriod = 1000;
        timer.start(heartbeatPeriod);
    }

    void loop()
    {
        if (timer.hasFiredOnce()) {
            heartbeatExp++;
            int exp = sendExp + heartbeatExp;
            float _rate = (exp) ? (float)(sendAct + heartbeatAct) / (exp) : 0;
            rate = constrain(_rate * 100, 0, 100);

            if (span++ > 10) {
                heartbeatAct = heartbeatExp = 0;
                sendAct = sendExp = 0;
                span = 0;
            }
            timer.start(heartbeatPeriod);
        }
    }
};

class PadTransport
{
private:
    const char *HEARTBEAT = "heartbeat";
    uint8_t payload[MAX_PACKET];
    Wireless wireless;
    RCCNode *node;
    Qos qos;

    struct Node {
        uint8_t addr;
        char name[NAME_SIZE + 1];
    };

    struct Known {
        struct Node nodes[MAX_LOCO];
        int selected;
        int len;
    } known;

    bool introToKnown(char *payload)
    {
        char *type = strtok(payload, SEPARATOR_STR);
        char *addr = strtok(NULL, SEPARATOR_STR);
        char *name = strtok(NULL, SEPARATOR_STR);
        if (type && addr && name) {
            if (type[0] == NRF_TYPE_LOCO) {
                Node *n = &known.nodes[known.len];
                n->addr = atoi(addr);
                strncpy(n->name, name, NAME_SIZE);
                n->name[NAME_SIZE] = '\0';
                // log("New node: " + String(known.len) + " " + String(n->addr) + " " + String(n->name));
                known.len++;
            }
            return true;
        }
        return false;
    }

public:
    bool isLocal;

    PadTransport(RCCNode *node) : node(node) {};

    int getConnSuccessRate()
    {
        return qos.rate;
    }

    char *getSelectedName()
    {
        return known.nodes[known.selected].name;
    }

    void cycleSelected()
    {
        int s = known.selected;
        if (known.selected < known.len - 1)
            known.selected++;
        else
            known.selected = 0;
        if (s != known.selected)
            subsribe();
    }

    bool selectByAddress(uint8_t addr)
    {
        for (int i = 0; i < known.len; i++) {
            if (known.nodes[i].addr == addr) {
                int s = known.selected;
                known.selected = i;
                if (s != known.selected)
                    subsribe();
                return true;
            }
        }
        return false;
    }

    int getKnownLen()
    {
        return known.len;
    }

    bool isKnown(int addr)
    {
        for (int i = 0; i < known.len; i++) {
            if (addr == known.nodes[i].addr)
                return true;
        }
        return false;
    }

    bool isMine(int addr)
    {
        if (isLocal)
            return (addr == known.nodes[known.selected].addr);
        else
            return true;
    }

    void processLocalIntro(char *packet, int from)
    {
        if (known.len < MAX_LOCO) {
            introToKnown(packet);
            log("Local Intro: " + known.nodes[known.len].addr);
        }
    }

    void introduce()
    {
        String packet = String(NRF_INTRO) + NRF_TYPE_PAD + NRF_SEPARATOR + node->locoAddr +
                        NRF_SEPARATOR + node->locoName + NRF_SEPARATOR + VERSION;

        int size = packet.length();
        send((uint8_t *)packet.c_str(), size);
        log("Intro: " + packet);
    }

    void askToIntro(int addr)
    {
        Command cmd = {.code = NRF_INTRO, .value = 0};
        send(&cmd, addr);
    }

    void askListNodes()
    {
        Command cmd = {.code = NRF_LIST_NODES, .value = 0};
        send(&cmd);
    }

    void askHeartbeatPeriod()
    {
        String packet = String(NRF_GET_VALUE) + HEARTBEAT;
        send((uint8_t *)packet.c_str(), packet.length());
    }

    void processListNodes(char *packet)
    {
        known.len = 0;
        char *token = packet;
        bool isThereMore = true;
        while (known.len < MAX_LOCO && isThereMore) {
            isThereMore = introToKnown(token);
            token = NULL;
        }
        log("Updated lits of nodes, count:");
        log(known.len);
    }

    void subsribe()
    {
        uint8_t addr = 1; // if no nodes register try to subsribe to the 1st one
        if (known.selected < known.len)
            addr = known.nodes[known.selected].addr;
        Command cmd = {.code = NRF_SUB, .value = addr};
        send(&cmd);
        log("Subscribed to: " + String(addr));
    }

    void processHeartbeat(uint8_t *payload, uint16_t size, int from)
    {
        memcpy(&node->state, payload, size);
        qos.heartbeatAct++;
    }

    void processSetValue(char *data, uint16_t size)
    {
        payload[size] = 0;
        char *sep = strchr(data, NRF_SEPARATOR);
        if (sep) {
            *sep = 0;
            if (strcmp(data, HEARTBEAT) == 0) {
                qos.heartbeatPeriod = atoi(sep + 1);
            }
            log("Set value: " + String(data) + " " + String(sep + 1));
            node->onSetValue(data, sep + 1);
        }
    }

    void send(uint8_t *payload, uint8_t size, int to = -1)
    {
        if (to < 0)
            to = (isLocal) ? known.nodes[known.selected].addr : 0;
        qos.sendExp++;
        if (wireless.write(payload, size, to))
            qos.sendAct++;
        if (size == 2) {
            log(String(to) + ">" + String((char)payload[0]) + payload[1]);
        } else {
            log(String(to) + ">" + String((char)payload[0]) + " size:" + size);
        }
    }

    void send(Command *cmd, int to = -1)
    {
        send((uint8_t *)cmd, sizeof(*cmd), to);
    }

    void received(uint8_t *payload, uint16_t size, int from)
    {
        if (size < COMMAND_SIZE)
            return;
        payload[size] = 0;
        char *data = (char *)payload + CODE_SIZE;
        struct Command *command = (struct Command *)payload;
        log("<" + String((char)command->code) + " " + String(command->value));

        if (command->code == NRF_INTRO) {
            if (isLocal) {
                processLocalIntro(data, from);
                // askHeartbeatPeriod();
            } else {
                introduce();
                askListNodes();
            }
        } else if (isLocal && !isKnown(from)) {
            askToIntro(from);
        } else if (command->code == NRF_LIST_NODES) {
            processListNodes(data);
            subsribe();
            askHeartbeatPeriod();
        } else if (command->code == NRF_HEARTBEAT) {
            if (isMine(from))
                processHeartbeat(payload, size, from);
        } else if (command->code == NRF_THROTTLE) {
            if (isMine(from))
                node->state.throttle = command->value;
        } else if (command->code == NRF_DIRECTION) {
            if (isMine(from))
                node->state.direction = command->value;
        } else if (command->code == NRF_SET_FUNCTION) {
            if (isMine(from)) {
                char *sep = strchr(data, NRF_SEPARATOR);
                if (sep) {
                    *sep = 0;
                    uint8_t functionCode = (uint8_t)atoi(data);
                    bool activate = (atoi(sep + 1) != 0);
                    if (functionCode < 30) {
                        if (activate)
                            node->state.bitstate |= (1 << functionCode);
                        else
                            node->state.bitstate &= ~(1 << functionCode);
                    }
                }
            }
        } else if (command->code == NRF_LIST_FUNCTION_RES) {
            if (isMine(from)) {
                log("Function List: " + String(data));
            }
        } else if (command->code == NRF_SET_VALUE) {
            if ((isMine(from)) && (size >= CODE_SIZE + 1)) {
                processSetValue(data, size);
            }
        } else if (command->code == NRF_LIST_VALUE_RES) {
            if (isMine(from)) {
                log("List: " + String(data));
            }
        } else if (command->code == NRF_PING) {
            if (isMine(from)) {
                send(payload, size, from);
            }
        }
    }

    void begin()
    {
        memset(&known, 0, sizeof(known));
        wireless.begin(node->locoAddr);
        isLocal = (node->locoAddr == 0);
        known.selected = 0;
        qos.begin();
        if (isLocal) {
            askHeartbeatPeriod();
        } else {
            introduce();
            askListNodes();
        }
    }

    bool loop()
    {
        bool update = false;
        if (wireless.available()) {
            update = true;
            int from;
            uint16_t size = wireless.read(payload, sizeof(payload), &from);
            received(payload, size, from);
        }

        qos.loop();
        return update;
    }
};
