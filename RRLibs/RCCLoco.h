/*
 * Railroad communication
 * 
 *  
 */
#include "RCCState.h"
#include "Wireless.h"
#include "Storage.h"


class RCCLoco {
  private:
    static const char PACKET_REG = 'r';
    static const char PACKET_NORM = 'n';
    static const int FUNCTION_BASE = '@';
    static const int FUNCTION_END = FUNCTION_BASE + 14; //16 - 2 bits for direction
    const char *FIELDS = "Time Bitstate Lost Throttle Speed Disatnce Battery Temp Psi Water";

    Wireless *wireless;
    Storage *storage;

    void authorize(const int node, const char *name) {
      String packet = String(PACKET_REG) + " " + String(node) + " " + String(name) + " " + String(FIELDS);
      int size = packet.length();
      wireless->write(packet.c_str(), size);
      if (debug)
        Serial.println(String("Authorize: ") + packet);
    }

    void send() {
      state.packetType = PACKET_NORM;
      wireless->write(&state, sizeof(state));
    }

    void received(char code, float value) {
      if (debug)
        Serial.println("Got: " + String((int)code) + "/" + String(value));

      if (code >= FUNCTION_BASE && code < FUNCTION_END) {
        code -= FUNCTION_BASE;
        if (value)
          state.bitstate |= 1 << code;
        else
          state.bitstate &= ~(1 << code);
        if (storage)
          storage->save(state.bitstate);
        onFunction(code, value);
      } else if (code == 'r') {
          authorize(addr, name);
      } else if (code == 'd') {
          int direction = constrain((int)value, 0, 2);
          state.direction = direction;
      } else if (code == 't') {
          int throttle = constrain((int)value, 0, 100);
          state.throttle = throttle;
      }
    }

    void handleThrottle() {
      static int runtimeThrottle = 0;
      if (state.direction == 0) {
        state.throttle = 0;
        runtimeThrottle = 0;
      } else {
        if (state.slow) {
          if (runtimeThrottle < state.throttle)
            runtimeThrottle += increment;
          else if (runtimeThrottle > state.throttle)
            runtimeThrottle -= increment;
        } else {
          runtimeThrottle = state.throttle;
        }
      }
      onThrottle(state.direction, runtimeThrottle);
    }

  protected:
    int addr;
    char *name;
    int increment;
    Timer timer;

  public:
    LocoState state;
    bool debug;

    RCCLoco(Wireless *wireless, const int addr, const char *name, Storage *storage = NULL) :
      wireless(wireless), addr(addr), name(name), increment(1), timer(100), storage(storage) {};

    bool isTransmitting() {
      return wireless->isTransmitting();
    }
    
    virtual void onFunction(char code, bool value) {
    }

    virtual void onThrottle(uint16_t direction, uint16_t throttle) {
    }

    void setup() {
      wireless->setup(addr);
      authorize(addr, name);
      if (storage)
        state.bitstate = storage->restore();
      timer.restart();
    }

    void loop() {
      if (wireless->available()) {
        struct Command command;
        wireless->read(&command, sizeof(command));
        received(command.cmd, command.value);
      }
      if (timer.hasFired()) {
        handleThrottle();
        state.tick = (float)millis() / 100;
        state.lost = wireless->getLostRate();
        send();
      }
    }
};
