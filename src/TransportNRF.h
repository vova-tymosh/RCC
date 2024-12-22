/*
 * Railroad communication
 *
 *
 */
#include "Wireless.h"
#include "Timer.h"
#include "RCCLocoBase.h"

class TransportNRF
{
private:
    static const char PACKET_REG = 'r';
    static const char PACKET_NORM = 'n';

    int addr;
    const char *name;
    Timer timer;
    Wireless wireless;
    RCCLocoBase *loco;

    void log(String msg)
    {
        if (loco->debugLevel > 1)
            Serial.print(msg);
    }

    void authorize(const int node, const char *name)
    {
        String packet = String(PACKET_REG) + " " + VERSION + " " + LOCO_FORMAT +
                        " " + String(node) + " " + String(name) + " " +
                        String(FIELDS);
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
            authorize(addr, name);
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

    //TODO fix name/addr
    TransportNRF(RCCLocoBase *loco) : loco(loco), addr(1), name("t001"), timer(100) {};

    // bool isTransmitting()
    // {
    //     return wireless->isTransmitting();
    // }

    void begin()
    {
        wireless.setup(addr);
        authorize(addr, name);
        timer.restart();
    }

    void loop()
    {
        if (wireless.available()) {
            struct Command command;
            wireless.read(&command, sizeof(command));
            received(command.cmd, command.value);
        }
        if (timer.hasFired()) {
            // handleThrottle();
            // state.tick = (float)millis() / 100;
            // state.lost = wireless->getLostRate();
            send();
        }
    }
};
