#pragma once
#include <WiFi.h>
#include "Timer.h"
#include "ConfigWeb.h"
#include "Settings.h"
#include "esp32/TransportWT.h"
#include "esp32/TransportMQ.h"

class Transport
{
private:
    ConfigWeb configWeb;
    TransportClient *transportClient;
    RCCNode *loco;

public:
    Transport(RCCNode *loco) : loco(loco) {}

    void wifiAP(String wifissid, String wifipwd)
    {
        WiFi.softAP(wifissid, wifipwd);
        Serial.print("Started wifi as AP, SSID: ");
        Serial.println(wifissid);
    }

    void wifiConnect(String wifissid, String wifipwd)
    {
        WiFi.begin(wifissid, wifipwd);
        Serial.print("Connecting to wifi.");
        while (WiFi.status() != WL_CONNECTED) {
            delay(100);
            Serial.print(".");
        }
        Serial.println("Connected");
        Serial.print("WiFi IP: ");
        Serial.println(WiFi.localIP());
    }

    void begin()
    {
        String wifiap = settings.get("wifiap");
        String wifissid = settings.get("wifissid");
        String wifipwd = settings.get("wifipwd");
        String mqtt = settings.get("mqtt");

        if (wifiap == "ON")
            wifiAP(wifissid, wifipwd);
        else
            wifiConnect(wifissid, wifipwd);

        if (mqtt == "ON") {
            transportClient = &mqttClient;
        } else {
            transportClient = new WiThrottleClient();
        }
        transportClient->setLoco(loco);

        configWeb.begin();
        transportClient->begin();
    }

    void loop()
    {
        configWeb.loop();
        transportClient->loop();
    }
};
