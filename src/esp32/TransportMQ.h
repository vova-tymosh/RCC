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
const char *functionTopic = "cab/{0}/function/{1}";
const char *valueTopic = "cab/{0}/value/{1}";
const char *listTopic = "cab/{0}/keys";

const char *throttleAct = "throttle";
const char *directionAct = "direction";
const char *getFunctionAct = "function/get";
const char *setFunctionAct = "function/";
const char *getValueAct = "value/get";
const char *listAct = "value/list";
const char *setValueAct = "value/";
const char *directionFWD = "FORWARD";
const char *directionREV = "REVERSE";
const char *functionON = "ON";

void onMqttMessage(char *topic, byte *payload, unsigned int length);

class MqttClient
{
private:
    WiFiClient conn;
    String valuesTopicUpdated;
    unsigned long nextReconnectTime;
    Timer heartbeatTimer;
    char heartbeatPayload[128];

public:
    PubSubClient mqtt;
    RCCLocoBase *loco;

    MqttClient() : mqtt(conn), heartbeatTimer(1000) {};

    void setLoco(RCCLocoBase *_loco)
    {
        loco = _loco;
    }

    void heartbeat()
    {
        LocoState *s = &loco->state;
        snprintf(heartbeatPayload, sizeof(heartbeatPayload),
                 "%d %d %d %d %d %d %d %d %d %d %d", s->tick, s->distance,
                 s->bitstate, s->speed, s->lost, s->throttle, s->throttle_out,
                 s->battery, s->temperature, s->psi, s->current);
        mqtt.publish(valuesTopicUpdated.c_str(), heartbeatPayload);
    }

    void introduce()
    {
        String topic(keysTopic);
        topic.replace("{0}", loco->locoAddr);
        String value = Keys[0];
        for (int i = 1; i < sizeof(Keys) / sizeof(char *); i++) {
            value += " ";
            value += Keys[i];
        }
        mqtt.publish(topic.c_str(), value.c_str(), true);
    }

    void reconnect()
    {
        if (millis() >= nextReconnectTime) {
            String brokerIP = settings.get("broker");
            String brokerPort = settings.get("brokerport");
            mqtt.setServer(brokerIP.c_str(), brokerPort.toInt());
            if (mqtt.connect(loco->locoName.c_str())) {
                Serial.println("[MQ] Connected");
                String topic(rootTopic);
                topic.replace("{0}", loco->locoAddr);
                mqtt.subscribe(topic.c_str());
                introduce();
            } else {
                Serial.println("[MQ] Failed to connect");
                nextReconnectTime = millis() + 5000;
            }
        }
    }

    void begin()
    {
        mqtt.setCallback(onMqttMessage);
        valuesTopicUpdated = valuesTopic;
        valuesTopicUpdated.replace("{0}", loco->locoAddr);
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

void onMqttMessage(char *topic, byte *payload, unsigned int length)
{
    char *value = (char *)payload;
    char *action = parseAction(topic);
    if (action == NULL)
        return;
    if (strcmp(action, throttleAct) == 0) {
        // Throttle/speed
        int l = (length > 4) ? 4 : length;
        char throttle[5];
        strncpy(throttle, value, l);
        throttle[l] = '\0';
        mqttClient.loco->setThrottle(atoi(throttle));
    } else if (strcmp(action, directionAct) == 0) {
        // Direction
        if (strncmp(value, directionFWD, length) == 0) {
            mqttClient.loco->setDirection(1);
        } else if (strncmp(value, directionREV, length) == 0) {
            mqttClient.loco->setDirection(0);
        } else {
            mqttClient.loco->setThrottle(0);
            mqttClient.loco->setDirection(1, true);
        }
    } else if (strcmp(action, getValueAct) == 0) {
        // Value, get state
        String key(payload, length);
        String value = mqttClient.loco->getValue((char *)key.c_str());
        String topic = valueTopic;
        topic.replace("{0}", mqttClient.loco->locoAddr);
        topic.replace("{1}", key);
        mqttClient.mqtt.publish(topic.c_str(), value.c_str());
    } else if (strcmp(action, listAct) == 0) {
        // Value, list all Keys (config and runtime)
        String topic = listTopic;
        topic.replace("{0}", mqttClient.loco->locoAddr);
        String value = mqttClient.loco->listValues();
        mqttClient.mqtt.publish(topic.c_str(), value.c_str());
    } else if (strncmp(action, setValueAct, strlen(setValueAct)) == 0) {
        // Value, set state. Hast to be the last one, after "get" and "list"
        char *key = action + strlen(setValueAct);
        String value(payload, length);
        mqttClient.loco->putValue(key, (char *)value.c_str());    
    } else if (strcmp(action, getFunctionAct) == 0) {
        // Function, get state. 
        String key(payload, length);
        int functionCode = key.toInt();
        String value =
            mqttClient.loco->getFunction(functionCode) ? "ON" : "OFF";
        String topic = functionTopic;
        topic.replace("{0}", mqttClient.loco->locoAddr);
        topic.replace("{1}", key);
        mqttClient.mqtt.publish(topic.c_str(), value.c_str());
    } else if (strncmp(action, setFunctionAct, strlen(setFunctionAct)) == 0) {
        // Function, set state. Hast to be the last one, after "get"
        action += strlen(setFunctionAct);
        int functionCode = atoi(action);
        if (length && strncmp(value, functionON, length) == 0)
            mqttClient.loco->setFunction(functionCode, true);
        else
            mqttClient.loco->setFunction(functionCode, false);
    }
}
