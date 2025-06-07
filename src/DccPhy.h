/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
#pragma once

typedef enum { Preamble, Seperator, SendByte } dccPulseState;

class dccPhy
{
private:
    static void writeHigh();
    static void writeLow();

public:
    static void begin();
    static void handleInterrupt();
};
