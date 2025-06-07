/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
/*
 *
 *
 *
 *
 */
#pragma once
#include <Wire.h>

class Intercom
{
private:
    static const int I2C_CODE = 1;
    static const int PACKET_LEN = 16;
    bool master;
    uint8_t size;
    uint8_t packet_in[PACKET_LEN];
    uint8_t packet_out[PACKET_LEN];

public:
    int onReceive();
    void onSend();

    void begin(bool master);
    void send(void *data, uint8_t size);
    void recv(void *data, uint8_t size);
};

extern Intercom intercom;
