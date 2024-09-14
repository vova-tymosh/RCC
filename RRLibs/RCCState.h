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
//  As of now it is 20
struct __attribute__((packed)) LocoState {
  uint8_t packetType;
  uint32_t tick;
  uint32_t distance;
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
  uint16_t speed;
  uint8_t lost;
  uint8_t throttle;
  uint8_t throttle_out;
  uint8_t battery;
  uint8_t temperature;
  uint8_t psi;
  uint8_t water;
};
 
