/*
 * Railroad communication
 *
 *
 */
#include "nrf52/Wireless.h"
#include "Timer.h"
#include "RCCLocoBase.h"
#include "Settings.h"

//TODO: tick
//TODO: clean up the protocol and bring it close to MQ/CLI
//TODO: remove contrains from throttle and direction

void printHex(uint8_t *payload, int size)
{
    Serial.print("@@@:"); 
    for (int i = 0; i < size; i++) {
        Serial.print(payload[i], HEX);
        Serial.print(" ");
    }
    Serial.println("$");
}

struct __attribute__((packed)) Command {
    uint8_t code;
    union {
        uint8_t value;
        uint8_t keySize;
        struct {
            uint8_t functionId : 7;
            uint8_t activate   : 1;
        };
    };
};
#define HEADER_SIZE sizeof(struct Command)

class Transport
{
private:
    static const char CMD_AUTH = 'A';
    static const char CMD_HEARTBEAT = 'H';

    static const char CMD_THROTTLE = 'T';
    static const char CMD_DIRECTION = 'D';
    static const char CMD_SET_FUNCTION = 'F';
    static const char CMD_GET_FUNCTION = 'P';
    static const char CMD_SET_VALUE = 'S';
    static const char CMD_GET_VALUE = 'G';
    static const char CMD_LIST_VALUE = 'L';

    Timer heartbeatTimer;
    Wireless wireless;
    RCCLocoBase *loco;

    static const int MAX_LINE = 256;
    uint8_t payload[MAX_LINE];

public:

    Transport(RCCLocoBase *loco) : loco(loco), heartbeatTimer(5000) {};

    void log(String msg)
    {
        if (loco->debugLevel > 1)
            Serial.println(msg);
    }

    void authorize()
    {
        String packet = String(CMD_AUTH) + " " + VERSION + " " + LOCO_FORMAT +
                        " " + loco->locoAddr + " " + loco->locoName;
        for (int i = 0; i < sizeof(Keys) / sizeof(char *); i++) {
            packet += " ";
            packet += Keys[i];
        }
        int size = packet.length();
        wireless.write(packet.c_str(), size);
        log(String("Authorize: ") + packet);
    }

    void heartbeat()
    {
        loco->state.packet_type = CMD_HEARTBEAT;
        loco->state.tick = (float)millis() / 100;
        wireless.write(&loco->state, sizeof(loco->state));
    }

    void received(uint8_t *payload, uint16_t size)
    {
        if (size < HEADER_SIZE)
            return;
        struct Command *command = (struct Command *)payload;
        log("Got: " + String((char)command->code) + "/" + String(command->value));
        
        if (command->code == CMD_AUTH) {
            authorize();
        } else if (command->code == CMD_THROTTLE) {
            loco->setThrottle(command->value);
        } else if (command->code == CMD_DIRECTION) {
            loco->setDirection(command->value);
        } else if (command->code == CMD_SET_FUNCTION) {
            loco->setFunction(command->functionId, command->activate);
        } else if (command->code == CMD_GET_FUNCTION) {
            Command reply;
            reply.code = CMD_SET_FUNCTION;
            reply.functionId = command->functionId;
            reply.activate = loco->getFunction(command->functionId);
            printHex((uint8_t *)&reply, sizeof(reply));
            wireless.write(&reply, sizeof(reply));
        } else if (command->code == CMD_SET_VALUE) {
            if (size >= HEADER_SIZE + command->keySize + 1) {                
                payload[HEADER_SIZE + command->keySize] = 0;
                payload[size - 1] = 0;
                char *key = (char *)(payload + HEADER_SIZE);
                char *value = (char *)(key + command->keySize + 1);
                loco->putValue(key, value);
            }
        } else if (command->code == CMD_GET_VALUE) {
            if (size >= HEADER_SIZE + 1) {
                payload[size - 1] = 0;
                char *key = (char *)(payload + HEADER_SIZE);
                String value = loco->getValue(key);
                char reply[128];
                memcpy(reply, payload, size);
                memcpy(reply + size, value.c_str(), value.length());
                reply[0] = CMD_SET_VALUE;
                size += value.length() + 1;
                reply[size - 1] = 0;
                // printHex((uint8_t *)reply, size);
                wireless.write(reply, size);
            }
        } else if (command->code == CMD_LIST_VALUE) {
            String reply = String(CMD_LIST_VALUE) + loco->listValues();
            wireless.write(reply.c_str(), reply.length());
            Serial.println(String("List:") + reply);
        } else {
            loco->onCommand(command->code, command->value);
        }
    }

    void begin()
    {
        int addr = loco->locoAddr.toInt();
        wireless.setup(addr);
        authorize();
        heartbeatTimer.start();
    }

    void loop()
    {
        if (wireless.available()) {
            uint16_t size = wireless.read(payload, sizeof(payload));
            received(payload, size);

            // struct Command command;
            // uint8_t buffer[128];
            // wireless.read(buffer, sizeof(buffer));
            // memcpy(&command, buffer, sizeof(command));
            // if (command.cmd == 122) {
            //     char key[256];
            //     char value[256];
            //     Serial.println("###1>" + String(command.keySize) + " = " +
            //                    String(command.valueSize));
            //     memcpy(key, buffer + sizeof(command), command.keySize);
            //     memcpy(value, buffer + sizeof(command) + command.keySize,
            //            command.valueSize);
            //     Serial.println("###3>" + String(key) + " = " + String(value));
            // }
            // received(command.cmd, command.value);
        }

        if (heartbeatTimer.hasFired()) {
            // state.lost = wireless->getLostRate();
            heartbeat();
        }
    }
};
