#pragma once

const char NRF_INTRO = 'A';
const char NRF_SUB = 'B';
const char NRF_LIST_CAB = 'C';
const char NRF_HEARTBEAT = 'H';

const char NRF_THROTTLE = 'T';
const char NRF_DIRECTION = 'D';
const char NRF_SET_FUNCTION = 'F';
const char NRF_GET_FUNCTION = 'P';
const char NRF_SET_VALUE = 'S';
const char NRF_GET_VALUE = 'G';
const char NRF_LIST_VALUE = 'L';

const char NRF_TYPE_LOCO = 'L';
const char NRF_TYPE_KEYPAD = 'K';

const char NRF_SEPARATOR = ' ';

const int MAX_PACKET = 256;

struct __attribute__((packed)) Command {
    uint8_t code;
    union {
        uint8_t value;
        uint8_t keySize;
        struct {
            uint8_t functionId : 7;
            uint8_t activate   : 1;
        };
    };
};
#define HEADER_SIZE sizeof(struct Command)

int split(char *input, char **output, uint8_t size)
{
    int index = 0;
    char *token = strtok(input, " ");
    while (token && index < size) {
        output[index] = token;
        token = strtok(NULL, " ");
        index++;
    }
    return index;
}

struct PadControls {
    uint16_t throttle;
    uint16_t direction;
    uint16_t lost;
    uint32_t timerBase;
};
