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