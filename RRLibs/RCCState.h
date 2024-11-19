/*
 * Railroad communication
 *
 *
 */

const int FUNCTION_BASE = ' ';
const int FUNCTION_END = FUNCTION_BASE + 32 - 2; // 2 bits for direction

// LocoState list of filed and their Python struct format
//  Update every time LocoState changes. Update version too.
const char *VERSION = "0.1.7";
const char *FIELDS =
    "Time Disatnce Bitstate Speed Lost Throttle ThrOut Battery Temp Psi Water";
const char *LOCO_FORMAT = "BIIIHBBBBBBB";

struct __attribute__((packed)) Command {
    uint8_t cmd;
    float value;
};

// Message longer than 24 bytes will be fragmented
//  As of now it is 22
struct __attribute__((packed)) LocoState {
    uint8_t packet_type;
    uint32_t tick;
    uint32_t distance;
    union {
        uint32_t bitstate;
        struct {
            uint32_t lights    : 1;
            uint32_t bell      : 1;
            uint32_t whistle   : 1;
            uint32_t shortw    : 1;
            uint32_t cocks     : 1;
            uint32_t           : 3; //
            uint32_t mute      : 1;
            uint32_t           : 2;
            uint32_t brek      : 1;
            uint32_t           : 1;
            uint32_t couple    : 1;
            uint32_t           : 2; //
            uint32_t           : 7;
            uint32_t allaboard : 1; //
            // uint32_t           : 4;
            uint32_t slow      : 1;
            uint32_t pid       : 1;
            uint32_t direction : 2; //
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
