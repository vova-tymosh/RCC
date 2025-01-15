#pragma once
#include <WebServer.h>
#include "Storage.h"


class ConfigWeb
{
public:
    WebServer server;
    Storage &storage;

    ConfigWeb(Storage &storage) : server(80), storage(storage) {}

    void begin();
    
    void loop() {
        server.handleClient();
    }
};

extern ConfigWeb configWeb;
