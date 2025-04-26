/*
 * Railroad communication
 *
 *
 */
#pragma once
#include "nrf52/Wireless.h"
#include "nrf52/Transport.h"
#include "RCCNode.h"
#include "Protocol.h"
#include "Timer.h"

#define MAX_LOCO 5
#define NAME_SIZE 5

// TODO: add local mode tests
// TODO: in the mq test make heartbeat fast to speed up the test

#define log(msg)                                                               \
    {                                                                          \
        if (node->debugLevel > 2)                                              \
            Serial.println(String("[NR] ") + (msg));                           \
    };

struct Qos {
    int sendExp = 0;
    int sendAct = 0;
    int hearbeatAct = 0;
    int hearbeatExp = 0;
    int hearbeatPeriod = 0;
    int rate = 0;
    int span = 0;
    Timer timer;

    void begin()
    {
        hearbeatPeriod = 1000;
        timer.start(hearbeatPeriod);
    }

    void loop()
    {
        if (timer.hasFiredOnce()) {
            hearbeatExp++;
            int exp = sendExp + hearbeatExp;
            float _rate = (exp) ? (float)(sendAct + hearbeatAct) / (exp) : 0;
            rate = constrain(_rate * 100, 0, 100);

            if (span++ > 10) {
                hearbeatAct = hearbeatExp = 0;
                sendAct = sendExp = 0;
                span = 0;
            }
            timer.start(hearbeatPeriod);
        }
    }
};

class KeypadTransport
{
private:
    const char *HEARTBEAT = "heartbeat";
    uint8_t payload[MAX_PACKET];
    Wireless wireless;
    RCCNode *node;
    Qos qos;

    struct Node {
        uint8_t addr;
        char name[NAME_SIZE];
    };

    struct Known {
        struct Node nodes[MAX_LOCO];
        int selected;
        int len;
    } known;

public:
    bool isLocal;

    KeypadTransport(RCCNode *node) : node(node) {};

    int getConnSuccessRate()
    {
        return qos.rate;
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

    char *getSelectedName()
    {
        return known.nodes[known.selected].name;
    }

    void cycleSelected()
    {
        if (known.selected < known.len - 1)
            known.selected++;
        else
            known.selected = 0;
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

    void processLocalIntro(char *payload, uint16_t size, int from)
    {
        payload[size] = 0;
        char *buffer[5];
        int tokens = split(payload + 1, (char **)&buffer, sizeofarray(buffer));

        if (tokens > 3) {
            known.nodes[known.len].addr = atoi(buffer[1]);
            strncpy(known.nodes[known.len].name, buffer[2], NAME_SIZE);
            log("Local Intro: " + String(known.nodes[known.len].addr));
            known.len++;
        }
    }

    void introduce()
    {
        String packet = String(NRF_INTRO) + NRF_TYPE_KEYPAD + NRF_SEPARATOR +
                        node->locoAddr + NRF_SEPARATOR + "RCC_Keypad" +
                        NRF_SEPARATOR + VERSION;

        int size = packet.length();
        send((uint8_t *)packet.c_str(), size);
        log("Intro: " + packet);
    }

    void askToIntro(int addr)
    {
        Command cmd = {.code = NRF_INTRO, .value = 0};
        send(&cmd, addr);
    }

    void askListCabs()
    {
        Command cmd = {.code = NRF_LIST_CAB, .value = 0};
        send(&cmd);
    }

    void askHearbteat()
    {
        String packet = String(NRF_GET_VALUE) + HEARTBEAT;
        send((uint8_t *)packet.c_str(), packet.length());
    }

    void processListCabs(char *packet, uint16_t size)
    {
        packet[size] = 0;
        int index = 0;
        char *buffer[10 * 3];
        int tokens = split(packet + 1, (char **)&buffer, sizeofarray(buffer));
        for (int i = 0; i < tokens / 3; i++) {
            if (buffer[i * 3][0] == NRF_TYPE_LOCO) {
                known.nodes[index].addr = atoi(buffer[i * 3 + 1]);
                strncpy(known.nodes[index].name, buffer[i * 3 + 2], NAME_SIZE);
                index++;
            }
        }
        known.len = index;
    }

    void subsribe()
    {
        uint8_t addr = 1; // if no nodes register try to subsribe to the 1st one
        if (known.selected < known.len)
            addr = known.nodes[known.selected].addr;
        Command cmd = {.code = NRF_SUB, .value = addr};
        send(&cmd);
        log("Subscribe to " + addr);
    }

    void processHeartbeat(uint8_t *payload, uint16_t size, int from)
    {
        memcpy(&node->state, payload, size);
        qos.hearbeatAct++;
    }

    void processSetValue(char *payload)
    {
        char *buffer[2];
        int tokens = split((char *)payload + CODE_SIZE, (char **)&buffer,
                           sizeofarray(buffer));
        if (tokens >= 2) {
            char *key = buffer[0];
            char *value = buffer[1];
            if (strcmp(key, HEARTBEAT) == 0) {
                qos.hearbeatPeriod = atoi(value);
            }
            log("Set value: " + String(key) + " " + String(value));
        }
    }

    void received(uint8_t *payload, uint16_t size, int from)
    {
        if (size < COMMAND_SIZE)
            return;
        struct Command *command = (struct Command *)payload;
        log("<" + String((char)command->code) + " " + String(command->value));

        if (command->code == NRF_INTRO) {
            if (isLocal) {
                processLocalIntro((char *)payload, size, from);
            } else {
                introduce();
                askListCabs();
            }
            askHearbteat();
        } else if (isLocal && !isKnown(from)) {
            askToIntro(from);
        } else if (command->code == NRF_LIST_CAB) {
            processListCabs((char *)payload, size);
            subsribe();
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
                if (command->functionId < 30)
                    if (command->activate)
                        node->state.bitstate |= (1 << command->functionId);
                    else
                        node->state.bitstate &= ~(1 << command->functionId);
            }
        } else if (command->code == NRF_SET_VALUE) {
            if ((isMine(from)) && (size >= CODE_SIZE + 1)) {
                payload[size] = 0;
                processSetValue((char *)payload);
            }
        } else if (command->code == NRF_LIST_VALUE_RES) {
            if (isMine(from)) {
                payload[size - 1] = 0;
                log("List: " + String((const char *)payload));
            }
        }
    }

    void begin()
    {
        memset(&known, 0, sizeof(known));
        int addr = node->locoAddr.toInt();
        wireless.begin(addr);
        isLocal = (addr == 0);
        known.selected = 0;
        qos.begin();
        if (!isLocal) {
            introduce();
            askListCabs();
        }
        askHearbteat();
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
