/*
 *
 *
 *
 */
#include "Intercom.h"

Intercom intercom;

void _onReceive(int bytes)
{
    intercom.onReceive();
}

void _onSend()
{
    intercom.onSend();
}

int Intercom::onReceive()
{
    uint8_t size = 0;
    if (Wire.available()) {
        size = Wire.read();
    }
    if (size > PACKET_LEN)
        size = PACKET_LEN;
    for (int i = 0; i < size; i++) {
        if (Wire.available())
            packet_in[i] = Wire.read();
    }
    return size;
}

void Intercom::onSend()
{
    Wire.write(size);
    Wire.write((const uint8_t *)packet_out, size);
}

void Intercom::setup(bool master)
{
    this->master = master;
    if (master) {
        Wire.begin();
    } else {
        Wire.begin(I2C_CODE);
        Wire.onReceive(_onReceive);
        Wire.onRequest(_onSend);
    }
}

void Intercom::send(void *data, uint8_t size)
{
    if (size > PACKET_LEN)
        size = PACKET_LEN;
    if (this->master) {
        Wire.beginTransmission(I2C_CODE);
        Wire.write(size);
        Wire.write((uint8_t *)data, size);
        Wire.endTransmission();
    } else {
        this->size = size;
        memcpy(packet_out, data, size);
    }
}

void Intercom::recv(void *data, uint8_t size)
{
    if (this->master) {
        Wire.requestFrom(I2C_CODE, PACKET_LEN + 1);
        size = onReceive();
    }
    memcpy(data, packet_in, size);
}
