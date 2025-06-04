#pragma once

#include <WebServer.h>

class ConfigWeb
{
private:
    static void handleRoot();

    static void handleSubmit();

public:
    static WebServer server;

    void begin();

    void loop()
    {
        server.handleClient();
    }
};
