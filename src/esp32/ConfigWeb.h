/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
#pragma once

#include <WebServer.h>

class ConfigWeb
{
private:
    static void handleRoot();

    static void handleSubmit();
    
    static void handleUpload();
    
    static void handleUploadComplete();
    
    static void handleFirmwareUpload();
    
    static void handleFirmwareComplete();

public:
    static WebServer server;

    void begin();

    void loop()
    {
        server.handleClient();
    }
};
