#pragma once
#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include "Timer.h"
#include "Settings.h"


//
// https://www.jmri.org/help/en/html/hardware/mqtt/index.shtml
//

RCCLocoBase *locoPointer;

char *getAction(char *topic)
{
    char *action = strchr(topic, '/');
    if (action != NULL) {
        action = strchr(++action, '/');
        if (action != NULL) {
            return ++action;
        }
    }
    return NULL;
}

void onMqttMessage(char* topic, byte* payload, unsigned int length)
{
    char *value = (char*)payload;
    char *action = getAction(topic);
    if (action == NULL)
        return;
    if (strcmp(action, "throttle") == 0) {
        int l = (length > 4) ? 4 : length;
        char throttle[5];
        strncpy(throttle, value, l);
        throttle[l] = '\0';
        locoPointer->setThrottle(atoi(throttle));
    } else if (strcmp(action, "direction") == 0) {
        if (strncmp(value, "FORWARD", length) == 0) {
            locoPointer->setDirection(1);
        } else if (strncmp(value, "REVERSE", length) == 0) {
            locoPointer->setDirection(0);
        } else {
            locoPointer->setDirection(1);
            locoPointer->setThrottle(0);
        }
    } else if (strncmp(action, "function/", strlen("function/")) == 0) {
        action += strlen("function/");
        int code = atoi(action);
        if (strncmp(value, "ON", length) == 0)
            locoPointer->setFunction(code, true);
        else 
            locoPointer->setFunction(code, false);
    }
}


class MqttClient
{
private:
    const char *BROCKER = "192.168.20.61";
    const int PORT = 1883;

    const char *rootTopic = "cab/{0}/#";
    const char *keysTopic = "cab/{0}/heartbeat/keys";
    const char *valuesTopic = "cab/{0}/heartbeat/values";
    String valuesTopicUpdated;

    WiFiClient conn;
    PubSubClient mqtt;

    String locoName;
    String locoAddr;
    Timer heartbeatTimer;
    unsigned long nextReconnectTime;

    char heartbeatPayload[256];
  
public:
    
    MqttClient(RCCLocoBase *loco): mqtt(conn), heartbeatTimer(1000) {
        locoPointer = loco;
    };

    void heartbeat()
    {
        LocoState *s = &locoPointer->state;
        snprintf(heartbeatPayload, sizeof(heartbeatPayload), "%d %d %d %d %d %d %d %d %d %d %d", 
            s->tick, s->distance, s->bitstate, s->speed, s->lost, 
            s->throttle, s->throttle_out, s->battery, s->temperature, s->psi, s->water);
        mqtt.publish(valuesTopicUpdated.c_str(), heartbeatPayload);
    }

    void authorize()
    {
        String topic(keysTopic);
        topic.replace("{0}", locoAddr);
        mqtt.publish(topic.c_str(), FIELDS, true);
    }

    void reconnect() {
        if (millis() >= nextReconnectTime) {
            if (mqtt.connect(locoName.c_str())) {
                Serial.println("[MQ] Connected");
                String topic(rootTopic);
                topic.replace("{0}", locoAddr);
                mqtt.subscribe(topic.c_str());
                authorize();
            } else {
                Serial.println("[MQ] Failed to connect");
                nextReconnectTime = millis() + 5000;
            }
        }
    }

    void begin()
    {
        mqtt.setServer(BROCKER, PORT);
        mqtt.setCallback(onMqttMessage);

        locoName = settings.get("loconame");
        locoAddr = settings.get("locoaddr");
        valuesTopicUpdated = valuesTopic;
        valuesTopicUpdated.replace("{0}", locoAddr);
        nextReconnectTime = millis();
    }

    void loop()
    {
        if (!mqtt.connected()) {
            reconnect();
        } else {
            if (heartbeatTimer.hasFired()) {
                heartbeat();
            }
        }
        mqtt.loop();
    }
};
