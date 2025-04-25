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


class KeypadTransport
{
private:
    Wireless wireless;
    RCCNode *node;

    Timer timer;
    bool localMode;

    struct Node {
        uint8_t addr;
        char name[NAME_SIZE];
    };

    struct Known {
        struct Node nodes[MAX_LOCO];
        int selected;
        int len;
    } known;

    // int lost;
    // int total;
    // int received;
    // bool alive;

    uint8_t payload[MAX_PACKET];

public:
    bool isLocal;

    KeypadTransport(RCCNode *node) : node(node), timer(100) {};

    void log(String msg)
    {
        if (node->debugLevel > 1)
            Serial.println(msg);
    }

    void send(uint8_t *payload, uint8_t size, int to = -1)
    {
        if (to < 0)
            to = (isLocal) ? known.nodes[known.selected].addr : 0;
        wireless.write(payload, size, to);
        Serial.print("[NR] >");
        Serial.print(to);
        Serial.print(">");
        printHex(payload, size);
        Serial.println("");
    }

    void send(Command *cmd, int to = -1)
    {
        send((uint8_t *)cmd, sizeof(*cmd), to);
    }

    // uint16_t getLostRate()
    // {
    //     uint16_t lostRate = 0;
    //     if (total) {
    //         lostRate = 100 * lost / total;
    //         if (lostRate > 100)
    //             lostRate = 100;
    //     }
    //     return lostRate;
    // }
    // bool isAlive()
    // {
    //     return alive;
    // }

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
        Serial.println("cycleSelected " + String(known.selected) + " " +
                       String(known.len));
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

    void processIntro(char *payload, uint16_t size, int from)
    {
        payload[size] = 0;
        char *buffer[5];
        Serial.println("Local intro " + String(payload));
        int tokens = split(payload + 1, (char**)&buffer, sizeofarray(buffer));
   
        if (tokens > 3) {
            known.nodes[known.len].addr = atoi(buffer[1]);
            strncpy(known.nodes[known.len].name, buffer[2], NAME_SIZE);
            known.len++;
        }
        Serial.println("Intro end " + String(known.len) + " " +
                       String(known.nodes[0].addr));
    }

    void introduce()
    {
        String packet = String(NRF_INTRO) + NRF_TYPE_KEYPAD + NRF_SEPARATOR +
                        node->locoAddr + NRF_SEPARATOR + "RCC_Keypad" + NRF_SEPARATOR + VERSION;

        int size = packet.length();
        send((uint8_t *)packet.c_str(), size);
        log(String("Intro: ") + packet);
    }

    void askToIntro(int addr)
    {
        Command cmd = {.code = NRF_INTRO, .value = 0};
        send(&cmd, addr);
        Serial.println("Ask to intro " + String(addr));
    }

    void askListCabs()
    {
        Command cmd = {.code = NRF_LIST_CAB, .value = 0};
        send(&cmd);
    }

    void processListCabs(char *packet, uint16_t size)
    {
        packet[size] = 0;
        int index = 0;
        char *buffer[10*3];
        int tokens = split(packet + 1, (char**)&buffer, sizeofarray(buffer));
        for (int i = 0; i < tokens/3; i++) {
            if (buffer[i*3][0] == NRF_TYPE_LOCO) {
                known.nodes[index].addr = atoi(buffer[i*3 + 1]);
                strncpy(known.nodes[index].name, buffer[i*3 + 2], NAME_SIZE);
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
        Serial.println("Subscribe to " + String(addr));
    }

    bool processHeartbeat(uint8_t *payload, uint16_t size, int from)
    {
        bool mine = true;
        if (mine) {
            memcpy(&node->state, payload, size);
        }
        return mine;
    }

    void received(uint8_t *payload, uint16_t size, int from)
    {
        if (size < COMMAND_SIZE)
            return;
        struct Command *command = (struct Command *)payload;
        log("[NR] <" + String((char)command->code) + " " + String(command->value));


        if (command->code == NRF_INTRO) {
            if (isLocal) {
                processIntro((char *)payload, size, from);
            } else {
                introduce();
                askListCabs();
            }
        } else if (isLocal && !isKnown(from)){
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
                char *buffer[2];
                int tokens = split((char*)payload + CODE_SIZE, (char**)&buffer, sizeofarray(buffer));
                if (tokens >= 2) {
                    char *key = buffer[0];
                    char *value = buffer[1];
                    Serial.println(String("NRF_SET_VALUE: ") + key + " " + value);
                }
            }
        } else if (command->code == NRF_LIST_VALUE_RES) {
            if (isMine(from)) {
                payload[size - 1] = 0;
                Serial.println("NRF_LIST_VALUE_RES: " + String((const char*)payload));
            }
        }
    }

    void setup()
    {
        memset(&known, 0, sizeof(known));
        int addr = node->locoAddr.toInt();
        wireless.setup(addr);
        isLocal = (addr == 0);
        known.selected = 0;
    }

    bool loop()
    {
        bool update = false;
        if (wireless.available()) {
            int from;
            uint16_t size = wireless.read(payload, sizeof(payload), &from);
            received(payload, size, from);
        }

        // if (alive_period.hasFired()) {
        //     static int lastPackets;
        //     alive = received > lastPackets;
        //     lastPackets = received;
        // }
        return update;
    }
};
