/*
 * Railroad communication
 * 
 *  
 */
#include "RCCState.h"
#include "Wireless.h"
#include "Storage.h"
#include "SpeedControl.h"


class RCCLoco {
  private:
    static const char PACKET_REG = 'r';
    static const char PACKET_NORM = 'n';

    Wireless *wireless;
    Storage *storage;


    void authorize(const int node, const char *name) {
      String packet = String(PACKET_REG) + " " + VERSION + " " + LOCO_FORMAT
        + " " + String(node) + " " + String(name) + " " + String(FIELDS);
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
          state.bitstate |= (uint32_t)1 << code;
        else
          state.bitstate &= ~((uint32_t)1 << code);
        if (storage)
          storage->save(state.bitstate);
        onFunction(code, value);
      } else if (code == 'r') {
          authorize(addr, name);
      } else if (code == 'd') {
          uint8_t direction = constrain((int)value, 0, 2);
          state.direction = direction;
      } else if (code == 't') {
          uint8_t throttle = constrain((int)value, 0, 100);
          state.throttle = throttle;
          if ((state.slow == false) && (state.pid == false))
            onThrottle(state.direction, state.throttle);
      } else if (code == 'a') {
          pid.setP(value);
          storage->save(toBinary(value), 1);
      } else if (code == 'b') {
          pid.setI(value);
          storage->save(toBinary(value), 2);
      } else if (code == 'c') {
          pid.setD(value);
          storage->save(toBinary(value), 3);
      } else if (code == 'e') {
          pid.setUpper(value);
          storage->save(toBinary(value), 4);
      } else {
        onCommand(code, value);
      }
    }

    void handleThrottle() {
      uint8_t throttle;
      if (state.direction == 0) {
        state.throttle = 0;
        state.throttle_out = 0;
        throttle = 0;
      } else {
        if (state.slow) {
          static uint8_t slowThrottle = 0;
          if (slowThrottle < state.throttle)
            slowThrottle += increment;
          else if (slowThrottle > state.throttle)
            slowThrottle -= increment;
          throttle = slowThrottle;
        } else {
          throttle = state.throttle;
        }
        if (state.pid) {
          float speed = state.speed;
          float scaled = pid.scale(speed);
          pid.setDesired(throttle);
          pid.setMeasured(scaled);
          state.throttle_out = pid.read();
          Serial.println("PID: " + String(speed) + " " + String(scaled) + " " + String(state.throttle_out));
        } else {
          state.throttle_out = throttle;
        }
      }
      onThrottle(state.direction, state.throttle_out);
    }

    float fromBinary(uint32_t x) {
      union {float f; uint32_t i;} t;
      t.i = x;
      return t.f;
    }

    uint32_t toBinary(float x) {
      union {float f; uint32_t i;} t;
      t.f = x;
      return t.i;
    }

  protected:
    int addr;
    const char *name;
    int increment;
    Timer timer;

  public:

    SpeedControl pid;

    LocoState state;
    bool debug;

    RCCLoco(Wireless *wireless, const int addr, const char *name, Storage *storage = NULL) :
      wireless(wireless), addr(addr), name(name), increment(1), timer(100), storage(storage) {};

    bool isTransmitting() {
      return wireless->isTransmitting();
    }
    
    virtual void onFunction(char code, bool value) {
    }

    virtual void onThrottle(uint8_t direction, uint8_t throttle) {
    }

    virtual void onCommand(char code, float value) {
    }

    void setup() {
      wireless->setup(addr);
      authorize(addr, name);
      float p = 0;
      float i = 0;
      float d = 0;
      float m = 0;
      if (storage) {
        state.bitstate = storage->restore(0);
        p = fromBinary(storage->restore(1));
        i = fromBinary(storage->restore(2));
        d = fromBinary(storage->restore(3));
        m = fromBinary(storage->restore(4));
      }
      pid.setup(p, i , d, m);
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
