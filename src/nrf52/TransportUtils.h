/*
 * Railroad communication
 *
 *
 */
#ifndef NRF52_PROTOCOL_H
#define NRF52_PROTOCOL_H

#define COMMAND_SIZE sizeof(struct Command)
#define CODE_SIZE 1
#define sizeofarray(x) (sizeof(x) / sizeof(x[0]))

const int MAX_PACKET = 144;

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

void printHex(uint8_t *payload, int size);

int split(char *input, char **output, uint8_t size, char delimiter);

#endif // NRF52_PROTOCOL_H