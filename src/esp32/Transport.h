#pragma once
#include <WiFi.h>
#include "Timer.h"
#include "ConfigWeb.h"
#include "Settings.h"

#if defined(RCC_NO_STATION)
    #include "esp32/TransportWT.h"
    #define TransportClient WiThrottleClient
#else
    #include "esp32/TransportMQ.h"
    #define TransportClient MqttClient
#endif

class Transport
{
private:
    ConfigWeb configWeb;
    TransportClient transportClient;

public:
    Transport(RCCLocoBase *loco) : transportClient(loco) {}

    void wifiAP(String wifissid, String wifipwd)
    {
        WiFi.softAP(wifissid, wifipwd);
        Serial.print("Started wifi as AP, SSID: ");
        Serial.println(wifissid);
    }

    void wifiConnect(String wifissid, String wifipwd)
    {
        WiFi.begin(wifissid, wifipwd);
        Serial.println("Connecting to wifi.");
        while (WiFi.status() != WL_CONNECTED) {
            delay(100);
            Serial.print(".");
        }
        Serial.print("Connected, IP: ");
        Serial.println(WiFi.localIP());
    }

    void begin()
    {
        String wifiap = settings.get("wifiap");
        String wifissid = settings.get("wifissid");
        String wifipwd = settings.get("wifipwd");

        if (wifiap == "on")
            wifiAP(wifissid, wifipwd);
        else
            wifiConnect(wifissid, wifipwd);

        configWeb.begin();
        transportClient.begin();
    }

    void loop()
    {
        configWeb.loop();
        transportClient.loop();
    }
};
