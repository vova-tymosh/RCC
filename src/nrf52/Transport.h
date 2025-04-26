/*
 * Railroad communication
 *
 *
 */
#include "nrf52/Wireless.h"
#include "RCCNode.h"
#include "Protocol.h"
#include "Timer.h"

void printHex(uint8_t *payload, int size);
int split(char *input, char **output, uint8_t size,
          char delimiter = NRF_SEPARATOR);

const int MAX_PACKET = 256;

#define COMMAND_SIZE sizeof(struct Command)
#define CODE_SIZE 1
#define sizeofarray(x) (sizeof(x) / sizeof(x[0]))

struct __attribute__((packed)) Command {
    uint8_t code;
    union {
        uint8_t value;
        struct {
            uint8_t functionId : 7;
            uint8_t activate   : 1;
        };
    };
};

class Transport
{
private:
    Timer heartbeatTimer;
    Wireless wireless;
    RCCNode *loco;
    uint8_t payload[MAX_PACKET];

public:
    Transport(RCCNode *loco, int heartbeatPeriod = 1000)
        : loco(loco), heartbeatTimer(heartbeatPeriod) {};

    void log(String msg)
    {
        if (loco->debugLevel > 1)
            Serial.println(msg);
    }

    void introduce()
    {
        String packet = String(NRF_INTRO) + NRF_TYPE_LOCO + NRF_SEPARATOR +
                        loco->locoAddr + NRF_SEPARATOR + loco->locoName +
                        NRF_SEPARATOR + VERSION + NRF_SEPARATOR + LOCO_FORMAT;

        for (int i = 0; i < sizeof(Keys) / sizeof(char *); i++) {
            packet += NRF_SEPARATOR;
            packet += Keys[i];
        }
        int size = packet.length();
        wireless.write(packet.c_str(), size);
        log(String("Intro: ") + packet);
    }

    void processList()
    {
        String packet = String(NRF_LIST_VALUE_RES) + loco->listValues();
        int size = packet.length();
        if (size > MAX_PACKET)
            size = MAX_PACKET;
        wireless.write(packet.c_str(), size);
        log(String("List: ") + packet);
    }

    void heartbeat()
    {
        loco->state.packet_type = NRF_HEARTBEAT;
        loco->state.tick = (float)millis() / 100;
        wireless.write(&loco->state, sizeof(loco->state));
    }

    void received(uint8_t *payload, uint16_t size)
    {
        if (size < COMMAND_SIZE)
            return;
        struct Command *command = (struct Command *)payload;
        log("[NR] <" + String((char)command->code) + " " +
            String(command->value));

        if (command->code == NRF_INTRO) {
            introduce();
        } else if (command->code == NRF_THROTTLE) {
            loco->setThrottle(command->value);
        } else if (command->code == NRF_DIRECTION) {
            loco->setDirection(command->value);
        } else if (command->code == NRF_SET_FUNCTION) {
            loco->setFunction(command->functionId, command->activate);
        } else if (command->code == NRF_GET_FUNCTION) {
            Command reply;
            reply.code = NRF_SET_FUNCTION;
            reply.functionId = command->functionId;
            reply.activate = loco->getFunction(command->functionId);
            wireless.write(&reply, sizeof(reply));
        } else if (command->code == NRF_SET_VALUE) {
            if (size >= CODE_SIZE + 1) {
                payload[size] = 0;
                char *buffer[2];
                int tokens = split((char *)payload + CODE_SIZE,
                                   (char **)&buffer, sizeofarray(buffer));
                if (tokens >= 2) {
                    char *key = buffer[0];
                    char *value = buffer[1];
                    loco->setValue(key, value);
                }
            }
        } else if (command->code == NRF_GET_VALUE) {
            if (size >= CODE_SIZE + 1) {
                payload[size] = 0;
                char *key = (char *)(payload + CODE_SIZE);
                String value = loco->getValue(key);
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
        } else {
            loco->onCommand(command->code, (char *)payload + CODE_SIZE, size);
        }
    }

    void begin()
    {
        int addr = loco->locoAddr.toInt();
        wireless.begin(addr);
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
            heartbeatTimer.start(loco->getHeartbeat());
        }
    }
};
