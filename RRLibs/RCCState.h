/*
 * Railroad communication
 * 
 *  
 */

struct __attribute__((packed)) Command {
  uint8_t cmd;
  float value;
};

// Message longer than 24 bytes will be fragmented
struct __attribute__((packed)) LocoState {
  uint8_t packetType;
  uint16_t tick;
  union {
    uint16_t bitstate;
    struct {
      uint16_t lights    : 1;
      uint16_t slow      : 1;
      uint16_t pid       : 1;
      uint16_t reserved  : 11;
      uint16_t direction : 2;
    };
  };
  uint16_t lost;
  uint16_t throttle;
  uint16_t speed;
  uint16_t disatnce;
  uint16_t battery;
  int16_t  temperature;
  uint16_t psi;
  uint16_t water;
};
 
