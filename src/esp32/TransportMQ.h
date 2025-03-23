#pragma once
#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include "Timer.h"
#include "Settings.h"


//
// https://www.jmri.org/help/en/html/hardware/mqtt/index.shtml
//

const char *rootTopic = "cab/{0}/#";
const char *keysTopic = "cab/{0}/heartbeat/keys";
const char *valuesTopic = "cab/{0}/heartbeat/values";
const char *throttleAction = "throttle";
const char *directionAction = "direction";
const char *directionFWD = "FORWARD";
const char *directionREV = "REVERSE";
const char *functionAction = "function/";
const char *functionOn = "ON";
const char *getAction = "get";
const char *valueTopic = "cab/{0}/value/{1}";
const char *putAction = "put";
const char *listAction = "list";
const char *listTopic = "cab/{0}/keys";

RCCLocoBase *locoPointer;
void onMqttMessage(char* topic, byte* payload, unsigned int length);


class MqttClient
{
private:
    String valuesTopicUpdated;
    WiFiClient conn;

    Timer heartbeatTimer;
    unsigned long nextReconnectTime;

    char heartbeatPayload[128];
public:
    PubSubClient mqtt;
    
    MqttClient(): mqtt(conn), heartbeatTimer(1000) {};

    void setLoco(RCCLocoBase *loco)
    {
        locoPointer = loco;
    }

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
        topic.replace("{0}", locoPointer->locoAddr);
        String value = Keys[0];
        for(int i = 1; i < sizeof(Keys)/sizeof(char*); i++) {
            value += " ";
            value += Keys[i];
        } 
        mqtt.publish(topic.c_str(), value.c_str(), true);
    }

    void reconnect() {
        if (millis() >= nextReconnectTime) {
            if (mqtt.connect(locoPointer->locoName.c_str())) {
                Serial.println("[MQ] Connected");
                String topic(rootTopic);
                topic.replace("{0}", locoPointer->locoAddr);
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
        String broker = settings.get("broker");
        String brokerPort = settings.get("brokerport");
        // mqtt.setServer(broker.c_str(), brokerPort.toInt());
        mqtt.setServer("192.168.20.61", brokerPort.toInt());
        mqtt.setCallback(onMqttMessage);

        valuesTopicUpdated = valuesTopic;
        valuesTopicUpdated.replace("{0}", locoPointer->locoAddr);
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

MqttClient mqttClient;

char *parseAction(char *topic)
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
    char *action = parseAction(topic);
    if (action == NULL)
        return;
    if (strcmp(action, throttleAction) == 0) {
        int l = (length > 4) ? 4 : length;
        char throttle[5];
        strncpy(throttle, value, l);
        throttle[l] = '\0';
        locoPointer->setThrottle(atoi(throttle));
    } else if (strcmp(action, directionAction) == 0) {
        if (strncmp(value, directionFWD, length) == 0) {
            locoPointer->setDirection(1);
        } else if (strncmp(value, directionREV, length) == 0) {
            locoPointer->setDirection(0);
        } else {
            locoPointer->setDirection(1);
            locoPointer->setThrottle(0);
        }
    } else if (strncmp(action, functionAction, strlen(functionAction)) == 0) {
        action += strlen(functionAction);
        int code = atoi(action);
        if (strncmp(value, functionOn, length) == 0)
            locoPointer->setFunction(code, true);
        else 
            locoPointer->setFunction(code, false);
    } else if (strcmp(action, getAction) == 0) {
        String key(payload, length);
        String value = locoPointer->getValue((char*)key.c_str());
        String topic = valueTopic;
        topic.replace("{0}", locoPointer->locoAddr);
        topic.replace("{1}", key);
        mqttClient.mqtt.publish(topic.c_str(), value.c_str());
    } else if (strcmp(action, putAction) == 0) {
        String key(payload, length);
        int separator = key.indexOf(':');
        if (separator > 0) {
            String value = key.substring(separator + 1);
            key = key.substring(0, separator);
            locoPointer->putValue((char*)key.c_str(), (char*)value.c_str());
        }
    } else if (strcmp(action, listAction) == 0) {
        String topic = listTopic;
        topic.replace("{0}", locoPointer->locoAddr);
        String value = locoPointer->listValues();
        mqttClient.mqtt.publish(topic.c_str(), value.c_str());
    }
}
