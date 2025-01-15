#pragma once
#include <WebServer.h>



class ConfigWeb
{
public:
    WebServer server;

    ConfigWeb() : server(80) {}

    void begin();

    void loop() {
        if (WiFi.status() != WL_CONNECTED)
            return; 
        server.handleClient();
    }
};

extern ConfigWeb configWeb;
