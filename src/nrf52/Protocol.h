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
const char NRF_LIST_VALUE_ASK = 'L';
const char NRF_LIST_VALUE_RES = 'J';

const char NRF_TYPE_LOCO = 'L';
const char NRF_TYPE_KEYPAD = 'K';

const char NRF_SEPARATOR = ' ';

const int MAX_PACKET = 256;

#define COMMAND_SIZE sizeof(struct Command)
#define CODE_SIZE 1
#define sizeofarray(x) (sizeof(x) / sizeof(x[0]))

struct __attribute__((packed)) Command {
    uint8_t code;
    union {
        uint8_t value;
        struct {
            uint8_t functionId : 7;
            uint8_t activate   : 1;
        };
    };
};

int split(char *input, char **output, uint8_t size, char delimiter = NRF_SEPARATOR)
{
    int index = 0;
    char d[2] = {delimiter, 0};
    char *token = strtok(input, d);
    while (token && index < size) {
        output[index] = token;
        token = strtok(NULL, d);
        index++;
    }
    return index;
}
