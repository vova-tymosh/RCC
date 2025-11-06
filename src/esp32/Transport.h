/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
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
    RCCNode *node;

    void setWifiExt()
    {
#ifdef CONFIG_IDF_TARGET_ESP32C6
        pinMode(WIFI_ENABLE, OUTPUT);
        digitalWrite(WIFI_ENABLE, LOW); // Activate RF switch control
        delay(100);
        pinMode(WIFI_ANT_CONFIG, OUTPUT);
        digitalWrite(WIFI_ANT_CONFIG, HIGH); // Use external antenna
#endif
    }

public:
    Transport(RCCNode *node) : node(node) {}

    void wifiAP(String wifissid, String wifipwd)
    {
        WiFi.softAP(wifissid, wifipwd);
        Serial.print("[WiFi] Started as AP, SSID: ");
        Serial.println(wifissid);
        node->onConnect(CONN_WIFI_AP);
    }

    void wifiConnect(String wifissid, String wifipwd)
    {
        WiFi.begin(wifissid, wifipwd);
        Serial.print("[WiFi] Connecting to " + wifissid);
        for (int i = 0; i < 600; i++) {
            if (WiFi.status() == WL_CONNECTED)
                break;
            delay(100);
            Serial.print(".");
        }
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("Connected");
            Serial.print("[WiFi] IP: ");
            Serial.println(WiFi.localIP());
            node->onConnect(CONN_WIFI);
        } else {
            Serial.println("\nFailed to connect, check credentials and if this is 2.4GHz wifi");
            Serial.println("Revert to AP");
            wifiAP(wifissid, wifipwd);
        }
    }

    void begin()
    {
        String wifiext = settings.get("wifiext");
        String wifiap = settings.get("wifiap");
        String wifissid = settings.get("wifissid");
        String wifipwd = settings.get("wifipwd");
        String mqtt = settings.get("mqtt");

        if (wifiext == "ON") {
            setWifiExt();
        }

        if (wifiap != "OFF")
            // If wifiap is ON or it is not defined, the system goes AP
            wifiAP(wifissid, wifipwd);
        else
            wifiConnect(wifissid, wifipwd);

        if (mqtt == "ON") {
            transportClient = &mqttClient;
        } else {
            transportClient = new WiThrottleClient();
        }
        transportClient->setLoco(node);

        configWeb.begin();
        transportClient->begin();
    }

    void loop()
    {
        configWeb.loop();
        transportClient->loop();
    }
};
