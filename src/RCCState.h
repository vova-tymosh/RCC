/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
/*
 * Railroad communication
 *
 *
 */
#pragma once

// System version, update every time the protocol changes
const char *VERSION = "0.7";

// Following structures and constants are connected and have to be kept in sync!
//   Failure to do so will result in communication errors

// Packet format for registration, look for Python Struct documentation. Has to
// be in the same order as LocoState
const char *LOCO_FORMAT = "BIIIHBBBBBBB";

// Key names for the LocoState fields. Has to be in the same order as LocoState
const char *Keys[] = {"Time", "Distance", "Bitstate", "Speed",
                      "Lost", "Throttle", "ThrOut",   "Battery",
                      "Temp", "Psi",      "Current"};

// The LocoState structure, represent realtime state and is sent as hearbeat to
// the Station
//  In case of NRF24/52 the message longer than 24 bytes will be fragmented
//  (longer delivery time) As of now it is 22
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
    uint8_t current;
};

// Offsets of the LocoState fields, used to access the fields in the LocoState
// structure
const size_t ValueOffsets[] = {
    offsetof(LocoState, tick),         offsetof(LocoState, distance),
    offsetof(LocoState, bitstate),     offsetof(LocoState, speed),
    offsetof(LocoState, lost),         offsetof(LocoState, throttle),
    offsetof(LocoState, throttle_out), offsetof(LocoState, battery),
    offsetof(LocoState, temperature),  offsetof(LocoState, psi),
    offsetof(LocoState, current)};
