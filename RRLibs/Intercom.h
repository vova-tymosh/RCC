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
    uint8_t packetIn[PACKET_LEN];
    uint8_t packetOut[PACKET_LEN];

public:
    int onReceive();
    void onSend();

    void setup(bool master);
    void send(void *data, uint8_t size);
    void recv(void *data, uint8_t size);
};

extern Intercom intercom;
