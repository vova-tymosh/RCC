/*
 * Railroad communication
 *
 *
 */
#pragma once

// LocoState list of filed and their Python struct format
//  Update every time LocoState changes. Update version too.
const char *VERSION = "0.1.9";
const char *FIELDS =
    "Time Disatnce Bitstate Speed Lost Throttle ThrOut Battery Temp Psi Water";
const char *LOCO_FORMAT = "BIIIHBBBBBBB";

// Message longer than 24 bytes will be fragmented
//  As of now it is 22
struct __attribute__((packed)) LocoState {
    uint8_t packet_type;
    uint32_t tick;
    uint32_t distance;
    union {
        uint32_t bitstate;
        struct {
            uint32_t lights    : 1; // 0 - G1
            uint32_t bell      : 1;
            uint32_t whistle   : 1;
            uint32_t shortw    : 1;
            uint32_t cocks     : 1; // 4
            uint32_t           : 1; // 5 - G2
            uint32_t           : 1;
            uint32_t           : 1;
            uint32_t mute      : 1; // 8
            uint32_t           : 1; // 9 - G3
            uint32_t           : 1;
            uint32_t brek      : 1;
            uint32_t           : 1; // 12
            uint32_t couple    : 1; // 13 - G3
            uint32_t           : 1;
            uint32_t           : 1;
            uint32_t           : 1;
            uint32_t           : 1;
            uint32_t           : 1;
            uint32_t           : 1;
            uint32_t           : 1; // 20
            uint32_t           : 1; // 21 - G4
            uint32_t           : 1;
            uint32_t allaboard : 1; // 23
            uint32_t           : 1;
            uint32_t           : 1;
            uint32_t           : 1;
            uint32_t           : 1; // 27 - END
            uint32_t slow      : 1; // 28
            uint32_t pid       : 1; // 29
            uint32_t direction : 2; // 30-31
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
