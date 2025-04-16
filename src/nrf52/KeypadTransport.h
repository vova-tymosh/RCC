/*
 * Railroad communication
 *
 *
 */
#pragma once
#include "KeypadState.h"
#include "Timer.h"
#include "nrf52/Wireless.h"

#define MAX_PACKET 128
#define MAX_LOCO 5
#define NAME_SIZE 5

struct __attribute__((packed)) CommandExt {
    uint8_t type;
    uint8_t cmd;
    float value;
};
struct __attribute__((packed)) Auth {
    uint8_t cmd;
    uint8_t addr;
};

class KeypadTransport
{
private:
    static const char PACKET_LOCO_AUTH = 'r';
    static const char PACKET_THR_AUTH = 'q';
    static const char PACKET_THR_SUB = 's';
    static const char PACKET_THR_NORM = 'p';
    static const char PACKET_LOCO_NORM = 'n';

    const int FUNCTION_BASE = ' ';
    const int FUNCTION_END = FUNCTION_BASE + 32 - 2; // 2 bits for direction

    Wireless *wireless;
    CommandExt command;
    Timer timer;
    Timer alive_period;
    int node;

    struct AvailableLoco {
        uint8_t addr;
        char name[NAME_SIZE];
    };

    struct RegisteredLoco {
        struct AvailableLoco locos[MAX_LOCO];
        int selected;
        int len;
    } registered;

    int lost;
    int total;
    int received;
    bool alive;

public:
    ThrComms() : timer(100)
    {
        wireless = new Wireless();
    };

    uint16_t getLostRate()
    {
        uint16_t lostRate = 0;
        if (total) {
            lostRate = 100 * lost / total;
            if (lostRate > 100)
                lostRate = 100;
        }
        return lostRate;
    }
    bool isAlive()
    {
        return alive;
    }

    char *getSelectedName()
    {
        return registered.locos[registered.selected].name;
    }

    int getSelectedAddr()
    {
        return registered.locos[registered.selected].addr;
    }

    void cycleSelected()
    {
        if (registered.selected < registered.len - 1)
            registered.selected++;
        else
            registered.selected = 0;
        Serial.println("cycleSelected " + String(registered.selected) + " " +
                       String(registered.len));
    }

    void send(char cmd, float value)
    {
        command.type = PACKET_THR_NORM;
        command.cmd = cmd;
        command.value = value;
        Serial.println("Send " + String(cmd) + "/" + String(value));
    }

    void sendFunction(char function, int value)
    {
        function = FUNCTION_BASE + function;
        send(function, (float)value);
    }

    void subsribe()
    {
        int addr = 1; // if no locos register try to subsribe to the 1st one
        if (registered.selected < registered.len)
            addr = registered.locos[registered.selected].addr;
        command.type = PACKET_THR_SUB;
        command.cmd = addr;
        command.value = 0;
        Serial.println("Subscribe to " + String(addr));
    }

    void handleAuthorizeRequest(char *packet, uint16_t size)
    {
        Serial.println("Request to authorize");
        packet[size] = 0;
        int i = 0;
        char *token = strtok(packet, " ");
        while (token && i < MAX_LOCO) {
            registered.locos[i].addr = atoi(token);
            token = strtok(NULL, " ");
            if (token) {
                strcpy(registered.locos[i].name, token);
                token = strtok(NULL, " ");
            }
            i++;
        }
        registered.len = i;
    }

    void askToAuthorize(int from)
    {
        Command cmd = {PACKET_LOCO_AUTH, 0};
        wireless->write(&cmd, sizeof(cmd), from);
        Serial.println("Local mode. Ask to auth " + String(from));
    }

    bool isLocalMode()
    {
        return (node == 0);
    }

    bool isRegistered(int addr)
    {
        for (int i = 0; i < registered.len; i++) {
            if (addr == registered.locos[i].addr)
                return true;
        }
        return false;
    }

    void handleLocalAuth(char *payload, uint16_t size, int from)
    {
        payload[size] = 0;
        Serial.println("Reg " + String(payload));
        int i = registered.len;
        char *token = strtok(payload, " ");
        if (token)
            token = strtok(NULL, " "); // skip version
        if (token)
            token = strtok(NULL, " "); // skip format
        if (token) {
            registered.locos[i].addr = atoi(token);
            token = strtok(NULL, " ");
            if (token) {
                strcpy(registered.locos[i].name, token);
                token = strtok(NULL, " ");
                registered.len = ++i;
            }
        }
        Serial.println("Reg end " + String(registered.len) + " " +
                       String(registered.locos[0].addr));
    }

    bool handleNormal(char *payload, uint16_t size, int from)
    {
        bool mine = false;
        if (isLocalMode()) {
            if (from == getSelectedAddr())
                mine = true;
            else if (!isRegistered(from))
                askToAuthorize(from);
        } else {
            mine = true;
        }
        if (mine) {
            memcpy(&loco, payload, size);
            Serial.println("Update " + String(size) + "/" + String(loco.tick));
        }
        return mine;
    }

    void setup(int node)
    {
        memset(&registered, 0, sizeof(registered));
        this->node = node;
        wireless->setup(node);
        command.type = PACKET_THR_AUTH;
        timer.start();
        alive_period.start(1000);
    }

    bool loop()
    {
        bool update = false;
        if (wireless->available()) {
            char packet[MAX_PACKET];
            int from;
            uint16_t size = wireless->read(packet, sizeof(packet), &from);
            if (size > 1) {
                char cmd = packet[0];
                char *payload = packet + 1;
                switch (cmd) {
                case PACKET_THR_AUTH:
                    if (!isLocalMode()) {
                        handleAuthorizeRequest(payload, size - 1);
                        subsribe();
                    }
                    break;
                case PACKET_LOCO_NORM:
                    update = handleNormal(packet, size, from);
                    received++;
                    break;
                case PACKET_LOCO_AUTH:
                    handleLocalAuth(payload, size - 1, from);
                    break;
                }
            }
        }
        if (timer.hasFired()) {
            if (isLocalMode()) {
                int to = getSelectedAddr();
                Command cmd = {command.cmd, command.value};
                if (!wireless->write(&cmd, sizeof(cmd), to))
                    lost++;
            } else {
                if (!wireless->write(&command, sizeof(command)))
                    lost++;
            }
            total++;
        }
        if (alive_period.hasFired()) {
            static int lastPackets;
            alive = received > lastPackets;
            lastPackets = received;
        }
        return update;
    }
};
