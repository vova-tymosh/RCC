/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
#pragma once
#include <Wire.h>

class Tca6408a
{
private:
    static const int I2C_ADDR = 0x21;
    static const int I2C_ADDR_ALT = 0x20;
    static const int TCA_INPUT = 0x00;
    static const int TCA_OUTPUT = 0x01;
    static const int TCA_POLARITY = 0x02;
    static const int TCA_CONFIGURATION = 0x03;

    uint8_t addr;
    bool active = false;
    bool error = false;

    void write(uint8_t reg, uint8_t value)
    {
        Wire.beginTransmission(addr);
        Wire.write(reg);
        Wire.write(value);
        Wire.endTransmission();
    }

    uint8_t read(uint8_t reg)
    {
        uint8_t value = 0;
        Wire.beginTransmission(addr);
        int r = Wire.write(reg);
        Wire.endTransmission();
        if (r == 0) {
            error = true;
            return 0;
        }
        Wire.requestFrom(addr, 1);
        if (Wire.available())
            value = Wire.read();
        return value;
    }

public:
    Tca6408a(bool alternativeAddr = false) : addr(alternativeAddr ? I2C_ADDR_ALT : I2C_ADDR) {};

    bool begin()
    {
        Wire.begin();
        error = false;
        uint8_t value = read(TCA_CONFIGURATION);
        if (error) {
            Serial.println("Failed to find TCA6408A chip");
        } else {
            active = true;
        }
        return (value == 0xFF);
    }

    void setOutput(uint8_t bit)
    {
        if (active) {
            uint8_t value = read(TCA_CONFIGURATION);
            value &= ~bit;
            write(TCA_CONFIGURATION, value);
        }
    }

    void setValue(uint8_t bit, bool on)
    {
        if (active) {
            uint8_t value = read(TCA_OUTPUT);
            if (on)
                value |= bit;
            else
                value &= ~bit;
            write(TCA_OUTPUT, value);
        }
    }

    bool getValue(uint8_t bit)
    {
        uint8_t value = 0;
        if (active) {
            value = read(TCA_INPUT);
        }
        return (value & bit) != 0;
    }
};
