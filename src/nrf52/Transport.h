/*
 * Railroad communication
 *
 *
 */
#include "nrf52/Wireless.h"
#include "nrf52/Protocol.h"
#include "Timer.h"
#include "RCCLocoBase.h"



// void printHex(uint8_t *payload, int size)
// {
//     Serial.print("@@@:"); 
//     for (int i = 0; i < size; i++) {
//         Serial.print(payload[i], HEX);
//         Serial.print(" ");
//     }
//     Serial.println("$");
// }

class Transport
{
private:

    Timer heartbeatTimer;
    Wireless wireless;
    RCCLocoBase *loco;
    uint8_t payload[MAX_PACKET];

public:

    Transport(RCCLocoBase *loco) : loco(loco), heartbeatTimer(5000) {};

    void log(String msg)
    {
        if (loco->debugLevel > 1)
            Serial.println(msg);
    }

    void introduce()
    {
        String packet = String(NRF_INTRO) + " " + VERSION + " " + LOCO_FORMAT +
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
        loco->state.packet_type = NRF_HEARTBEAT;
        loco->state.tick = (float)millis() / 100;
        wireless.write(&loco->state, sizeof(loco->state));
    }

    void received(uint8_t *payload, uint16_t size)
    {
        if (size < HEADER_SIZE)
            return;
        struct Command *command = (struct Command *)payload;
        log("Got: " + String((char)command->code) + "/" + String(command->value));
        
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
            if (size >= HEADER_SIZE + command->keySize + 1) {                
                payload[HEADER_SIZE + command->keySize] = 0;
                payload[size - 1] = 0;
                char *key = (char *)(payload + HEADER_SIZE);
                char *value = (char *)(key + command->keySize + 1);
                loco->putValue(key, value);
            }
        } else if (command->code == NRF_GET_VALUE) {
            if (size >= HEADER_SIZE + 1) {
                payload[size - 1] = 0;
                char *key = (char *)(payload + HEADER_SIZE);
                String value = loco->getValue(key);
                char reply[MAX_PACKET];
                memcpy(reply, payload, size);
                memcpy(reply + size, value.c_str(), value.length());
                reply[0] = NRF_SET_VALUE;
                size += value.length() + 1;
                reply[size - 1] = 0;
                wireless.write(reply, size);
            }
        } else if (command->code == NRF_LIST_VALUE) {
            String reply = String(NRF_LIST_VALUE) + loco->listValues();
            wireless.write(reply.c_str(), reply.length());
            //TODO: remove
            Serial.println(String("List:") + reply);
        } else {
            loco->onCommand(command->code, command->value);
        }
    }

    void begin()
    {
        int addr = loco->locoAddr.toInt();
        wireless.setup(addr);
        introduce();
        heartbeatTimer.start();
    }

    void loop()
    {
        if (wireless.available()) {
            uint16_t size = wireless.read(payload, sizeof(payload));
            received(payload, size);
        }

        if (heartbeatTimer.hasFired()) {
            heartbeat();
        }
    }
};
