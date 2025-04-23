#pragma once
#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include "Timer.h"
#include "Settings.h"
#include "Protocol.h"


//
// https://www.jmri.org/help/en/html/hardware/mqtt/index.shtml
//



void onMqttMessage(char *topic, byte *payload, unsigned int length);

class MqttClient
{
private:
    WiFiClient conn;
    String heartbeatTopic;
    unsigned long nextReconnectTime;
    Timer heartbeatTimer;
    char heartbeatPayload[128];

public:
    PubSubClient mqtt;
    RCCNode *loco;
    String topicPrefix;

    MqttClient() : mqtt(conn), heartbeatTimer(1000) {};

    void setLoco(RCCNode *_loco)
    {
        loco = _loco;
    }

    void heartbeat()
    {
        LocoState *s = &loco->state;
        String heartbeatPayload;
        heartbeatPayload.reserve(64);
        heartbeatPayload = String(s->tick) + MQ_SEPARATOR +
                            String(s->distance) + MQ_SEPARATOR +
                            String(s->bitstate) + MQ_SEPARATOR +
                            String(s->speed) + MQ_SEPARATOR +
                            String(s->lost) + MQ_SEPARATOR +
                            String(s->throttle) + MQ_SEPARATOR +
                            String(s->throttle_out) + MQ_SEPARATOR +
                            String(s->battery) + MQ_SEPARATOR +
                            String(s->temperature) + MQ_SEPARATOR +
                            String(s->psi) + MQ_SEPARATOR +
                            String(s->current);
        mqtt.publish(heartbeatTopic.c_str(), heartbeatPayload.c_str());
    }

    void introduce()
    {
        String topic = topicPrefix + String(MQ_INTRO);
        String value = Keys[0];
        for (int i = 1; i < sizeof(Keys) / sizeof(char *); i++) {
            value += MQ_SEPARATOR;
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
                String topic = topicPrefix + "#";
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
        topicPrefix = String(MQ_PREFIX) + "/" + loco->locoAddr + "/";
        heartbeatTopic = topicPrefix + String(MQ_HEARTBEAT_VALUES);
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
    if (strcmp(action, MQ_SET_THROTTLE) == 0) {
        // Throttle/speed
        int l = (length > 4) ? 4 : length;
        char throttle[5];
        strncpy(throttle, value, l);
        throttle[l] = '\0';
        mqttClient.loco->setThrottle(atoi(throttle));
    } else if (strcmp(action, MQ_SET_DIRECTION) == 0) {
        // Direction
        for (int i = 0; i < 4; i++) {
            if ((strncmp(value, MQ_DIRECTIONS[i], length) == 0) || (value[0] == '0' + i)) {
                mqttClient.loco->setDirection(i);
                break;
            }
        }
    } else if (strcmp(action, MQ_GET_VALUE) == 0) {
        // Value, get state
        String key(payload, length);
        String value = mqttClient.loco->getValue((char *)key.c_str());
        String topic = mqttClient.topicPrefix + MQ_SET_VALUE + key; 
        mqttClient.mqtt.publish(topic.c_str(), value.c_str());
    } else if (strcmp(action, MQ_LIST_VALUE_ASK) == 0) {
        // Value, list all Keys (config and runtime)
        String topic = mqttClient.topicPrefix + MQ_LIST_VALUE_RES;
        String value = mqttClient.loco->listValues();
        mqttClient.mqtt.publish(topic.c_str(), value.c_str());
    } else if (strncmp(action, MQ_SET_VALUE, strlen(MQ_SET_VALUE)) == 0) {
        // Value, set state. Hast to be the last one, after "get" and "list"
        char *key = action + strlen(MQ_SET_VALUE);
        String value(payload, length);
        mqttClient.loco->putValue(key, (char *)value.c_str());
    } else if (strcmp(action, MQ_GET_FUNCTION) == 0) {
        // Function, get state.
        String key(payload, length);
        int functionCode = key.toInt();
        String value =
            mqttClient.loco->getFunction(functionCode) ? MQ_ON : MQ_OFF;
        String topic = mqttClient.topicPrefix + MQ_SET_FUNCTION + key;
        mqttClient.mqtt.publish(topic.c_str(), value.c_str());
    } else if (strncmp(action, MQ_SET_FUNCTION, strlen(MQ_SET_FUNCTION)) == 0) {
        // Function, set state. Hast to be the last one, after "get"
        action += strlen(MQ_SET_FUNCTION);
        int functionCode = atoi(action);
        if (length && strncmp(value, MQ_ON, length) == 0)
            mqttClient.loco->setFunction(functionCode, true);
        else
            mqttClient.loco->setFunction(functionCode, false);
    }
}
