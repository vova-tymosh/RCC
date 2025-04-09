/*
 * Railroad communication
 *
 *
 */
#include "nrf52/Wireless.h"
#include "Timer.h"
#include "RCCLocoBase.h"
#include "Settings.h"

// TODO - tick, pid setting (CV?)

struct __attribute__((packed)) Command {
    uint8_t cmd;
    union {
        float value;
        struct {
            uint8_t keySize;
            uint8_t valueSize;
            uint16_t reserved;
        };
    };
};

class Transport
{
private:
    static const char PACKET_REG = 'r';
    static const char PACKET_NORM = 'n';
    const int FUNCTION_BASE = ' ';
    const int FUNCTION_END = FUNCTION_BASE + 32 - 2; // 2 bits for direction

    Timer timer;
    Wireless wireless;
    RCCLocoBase *loco;

    void log(String msg)
    {
        if (loco->debugLevel > 1)
            Serial.println(msg);
    }

    void authorize()
    {
        String packet = String(PACKET_REG) + " " + VERSION + " " + LOCO_FORMAT +
                        " " + loco->locoAddr + " " + loco->locoName;
        for (int i = 0; i < sizeof(Keys) / sizeof(char *); i++) {
            packet += " ";
            packet += Keys[i];
        }
        int size = packet.length();
        wireless.write(packet.c_str(), size);
        log(String("Authorize: ") + packet);
    }

    void send()
    {
        loco->state.packet_type = PACKET_NORM;
        wireless.write(&loco->state, sizeof(loco->state));
    }

    void received(char code, float value)
    {
        log("Got: " + String((int)code) + "/" + String(value));

        if (code >= FUNCTION_BASE && code < FUNCTION_END) {
            code -= FUNCTION_BASE;
            loco->setFunction(code, value != 0);
        } else if (code == 'r') {
            authorize();
        } else if (code == 'd') {
            uint8_t direction = constrain((int)value, 0, 2);
            loco->setDirection(direction);
        } else if (code == 't') {
            uint8_t throttle = constrain((int)value, 0, 100);
            loco->setThrottle(throttle);
            // } else if (code == 'a') {
            //     pid.setP(value);
            //     storage->save(toBinary(value), 1);
            // } else if (code == 'b') {
            //     pid.setI(value);
            //     storage->save(toBinary(value), 2);
            // } else if (code == 'c') {
            //     pid.setD(value);
            //     storage->save(toBinary(value), 3);
            // } else if (code == 'e') {
            //     pid.setUpper(value);
            //     storage->save(toBinary(value), 4);
        } else {
            loco->onCommand(code, value);
        }
    }

public:
    Transport(RCCLocoBase *loco)
        : loco(loco), timer(100) {

          };

    // bool isTransmitting()
    // {
    //     return wireless->isTransmitting();
    // }

    void begin()
    {
        int addr = loco->locoAddr.toInt();
        wireless.setup(addr);
        authorize();
        timer.start();
    }

    void loop()
    {
        if (wireless.available()) {
            struct Command command;
            uint8_t buffer[128];
            wireless.read(buffer, sizeof(buffer));
            memcpy(&command, buffer, sizeof(command));
            if (command.cmd == 122) {
                char key[256];
                char value[256];
                Serial.println("###1>" + String(command.keySize) + " = " +
                               String(command.valueSize));
                memcpy(key, buffer + sizeof(command), command.keySize);
                memcpy(value, buffer + sizeof(command) + command.keySize,
                       command.valueSize);
                Serial.println("###3>" + String(key) + " = " + String(value));
            }
            received(command.cmd, command.value);
        }
        if (timer.hasFired()) {
            // handleThrottle();
            // state.tick = (float)millis() / 100;
            // state.lost = wireless->getLostRate();
            send();
            // Serial.println("###loop>");
        }
    }
};
