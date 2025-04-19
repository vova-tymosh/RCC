/*
 * Railroad communication
 *
 *
 */
#pragma once
#include "nrf52/Wireless.h"
#include "nrf52/Protocol.h"
#include "Timer.h"
#include "RCCLocoBase.h"

#define MAX_LOCO 5
#define NAME_SIZE 5


void printHex(uint8_t *payload, int size)
{

    for (int i = 0; i < size; i++) {
        Serial.print(payload[i], HEX);
        Serial.print(" ");
    }
    Serial.println("");
}

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
    bool isLocalMode;

    KeypadTransport(RCCNode *node) : node(node), timer(100) {};

    void log(String msg)
    {
        if (node->debugLevel > 1)
            Serial.println(msg);
    }

    void send(uint8_t *payload, uint8_t size)
    {
        wireless.write(payload, size);
        // Serial.println("[MQ] >" + String((const char *)payload, (unsigned
        // int)size));
        Serial.print("[NR] >");
        printHex(payload, size);
    }

    void send(Command *cmd)
    {
        send((uint8_t *)cmd, sizeof(*cmd));
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

    // char *getSelectedName()
    // {
    //     return known.nodes[known.selected].name;
    // }

    // int getSelectedAddr()
    // {
    //     return known.nodes[known.selected].addr;
    // }

    // void cycleSelected()
    // {
    //     if (known.selected < known.len - 1)
    //         known.selected++;
    //     else
    //         known.selected = 0;
    //     Serial.println("cycleSelected " + String(known.selected) + " " +
    //                    String(known.len));
    // }

    // bool isRegistered(int addr)
    // {
    //     for (int i = 0; i < known.len; i++) {
    //         if (addr == known.nodes[i].addr)
    //             return true;
    //     }
    //     return false;
    // }

    void processIntro(char *payload, uint16_t size, int from)
    {
        payload[size] = 0;
        Serial.println("Reg " + String(payload));
        int i = known.len;
        char *token = strtok(payload, " ");
        if (token)
            token = strtok(NULL, " "); // skip version
        if (token)
            token = strtok(NULL, " "); // skip format
        if (token) {
            known.nodes[i].addr = from;
            token = strtok(NULL, " ");
            if (token) {
                strcpy(known.nodes[i].name, token);
                token = strtok(NULL, " ");
                known.len = ++i;
            }
        }
        Serial.println("Reg end " + String(known.len) + " " +
                       String(known.nodes[0].addr));
    }

    void introduce()
    {
        String packet = String(NRF_INTRO) + " " + NRF_TYPE_KEYPAD + " " +
                        node->locoAddr + " RCC_Keypad " + VERSION;

        int size = packet.length();
        send((uint8_t *)packet.c_str(), size);
        log(String("Authorize: ") + packet);
    }

    void askListCabs()
    {
        Command cmd = {.code = NRF_LIST_CAB, .value = 0};
        send(&cmd);
    }

    void processListCabs(char *packet, uint16_t size)
    {
        if (size < 2)
            return;
        packet[size] = 0;
        int index = 0;
        char *buffer[33];
        char nodeType = '\0';
        int tokens = split(packet + 1, (char**)&buffer, sizeof(buffer)/sizeof(char *));
        for (int i = 0; i < tokens; i++) {
            if (i % 3 == 0)
                nodeType = buffer[i][0];
            if (nodeType == NRF_TYPE_LOCO) {
                if (i % 3 == 1)
                    known.nodes[index].addr = atoi(buffer[i]);
                if (i % 3 == 2) {
                    strncpy(known.nodes[index].name, buffer[i], NAME_SIZE);
                    index++;
                }
            }
        }
        known.len = index;
        // Serial.println("Known len=" + String(known.len));
        // Serial.println(String("  ##") + known.nodes[0].addr + " " +
        //                known.nodes[0].name);
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
        // bool mine = false;
        // if (isLocalMode) {
        //     if (from == getSelectedAddr())
        //         mine = true;
        //     else if (!isRegistered(from))
        //         askToAuthorize(from);
        // } else {
        //     mine = true;
        // }

        bool mine = true;
        if (mine) {
            memcpy(&node->state, payload, size);
            Serial.println("Update " + String(size) + "/" +
                           String(node->state.tick));
        }
        return mine;
    }

    void received(uint8_t *payload, uint16_t size, int from)
    {
        if (size < HEADER_SIZE)
            return;
        struct Command *command = (struct Command *)payload;
        // log("Got: " + String((char)command->code) + "/" +
        // String(command->value));

        if (command->code == NRF_INTRO) {
            if (isLocalMode) {
                processIntro((char *)payload, size, from);
            } else {
                introduce();
                askListCabs();
            }
        } else if (command->code == NRF_LIST_CAB) {
            processListCabs((char *)payload, size);
            subsribe();
        } else if (command->code == NRF_HEARTBEAT) {
            processHeartbeat(payload, size, from);
        } else if (command->code == NRF_THROTTLE) {
            node->state.throttle = command->value;
        } else if (command->code == NRF_DIRECTION) {
            node->state.direction = command->value;
        } else if (command->code == NRF_SET_FUNCTION) {
            if (command->functionId < 30)
                if (command->activate)
                    node->state.bitstate |= (1 << command->functionId);
                else
                    node->state.bitstate &= ~(1 << command->functionId);
        } else if (command->code == NRF_SET_VALUE) {
            if (size >= HEADER_SIZE + command->keySize + 1) {
                payload[HEADER_SIZE + command->keySize] = 0;
                payload[size - 1] = 0;
                char *key = (char *)(payload + HEADER_SIZE);
                char *value = (char *)(key + command->keySize + 1);               
                Serial.println(String("NRF_SET_VALUE: ") + key + " " + value);
            }
        } else if (command->code == NRF_LIST_VALUE) {
            payload[size - 1] = 0;
            Serial.println("NRF_LIST_VALUE: " + String((const char*)payload));
        }
    }

    void setup()
    {
        memset(&known, 0, sizeof(known));
        int addr = node->locoAddr.toInt();
        wireless.setup(addr);
        isLocalMode = (addr == 0);
        known.selected = 0;
        // command.type = PACKET_THR_AUTH;
        // timer.start();
        // alive_period.start(1000);
    }

    bool loop()
    {
        bool update = false;
        if (wireless.available()) {
            int from;
            uint16_t size = wireless.read(payload, sizeof(payload), &from);
            received(payload, size, from);
        }

        // if (timer.hasFired()) {
        //     if (isLocalMode()) {
        //         int to = getSelectedAddr();
        //         Command cmd = {command.cmd, command.value};
        //         if (!wireless->write(&cmd, sizeof(cmd), to))
        //             lost++;
        //     } else {
        //         if (!wireless->write(&command, sizeof(command)))
        //             lost++;
        //     }
        //     total++;
        // }
        // if (alive_period.hasFired()) {
        //     static int lastPackets;
        //     alive = received > lastPackets;
        //     lastPackets = received;
        // }
        return update;
    }
};
