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
#ifndef NRF52_PROTOCOL_H
#define NRF52_PROTOCOL_H

#define COMMAND_SIZE sizeof(struct Command)
#define CODE_SIZE 1

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

inline char *strcatm(char *d, const char *s)
{
    strcat(d, s);
    d += strlen(s);
    return d;
}

inline char *strcatm(char *d, const char c)
{
    *d++ = c;
    *d = 0;
    return d;
}

inline char *strcatm(char *d, const int i)
{
    itoa(i, d, 10);
    d += strlen(d);
    return d;
}

#endif // NRF52_PROTOCOL_H