/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
#pragma once

#include "RCCNode.h"

class TransportClient
{
public:
    RCCNode *node;
    void setLoco(RCCNode *_loco)
    {
        node = _loco;
    }
    virtual void begin() = 0;
    virtual void loop() = 0;
};