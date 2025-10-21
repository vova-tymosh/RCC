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
#include <WiFiClient.h>
#include <PubSubClient.h>
#include "Timer.h"
#include "Settings.h"
#include "Protocol.h"
#include "Functions.h"
#include "esp32/TransportClient.h"

//
// https://www.jmri.org/help/en/html/hardware/mqtt/index.shtml
//

#if RCC_DEBUG >= 2
#define log(msg)                                                               \
    {                                                                          \
        Serial.print("[MQ] "); Serial.println(msg);                            \
    };
#else
#define log(msg)
#endif

void onMqttMessage(char *topic, byte *payload, unsigned int length);

class MqttClient : public TransportClient
{
private:
    WiFiClient conn;
    String heartbeatTopic;
    unsigned long nextReconnectTime;
    Timer heartbeatTimer;
    char heartbeatPayload[128];

public:
    PubSubClient mqtt;
    String topicPrefix;
    Functions functions;

    MqttClient() : mqtt(conn), heartbeatTimer(1000) {};

    void write(String topic, String message)
    {
        log(String(">") + topic + "+" + message);
        mqtt.publish(topic.c_str(), message.c_str());
    }

    void heartbeat()
    {
        LocoState *s = &node->state;
        s->tick = (float)millis() / 100;
        String heartbeatPayload;
        heartbeatPayload.reserve(64);
        heartbeatPayload =
            String(s->tick) + MQ_SEPARATOR + String(s->distance) +
            MQ_SEPARATOR + String(s->bitstate) + MQ_SEPARATOR +
            String(s->speed) + MQ_SEPARATOR + String(s->lost) + MQ_SEPARATOR +
            String(s->throttle) + MQ_SEPARATOR + String(s->throttle_out) +
            MQ_SEPARATOR + String(s->battery) + MQ_SEPARATOR +
            String(s->temperature) + MQ_SEPARATOR + String(s->psi) +
            MQ_SEPARATOR + String(s->current);
        write(heartbeatTopic, heartbeatPayload);
    }

    void heartBeatKeys()
    {
        String topic = topicPrefix + String(MQ_HEARTBEAT_KEYS);
        String value;
        value.reserve(256);
        for (int i = 0; i < sizeofarray(Keys) - 1; i++) {
            value += Keys[i];
            value += MQ_SEPARATOR;
        }
        value += Keys[sizeofarray(Keys) - 1];
        write(topic, value);
    }

    void introduce()
    {
        String topic = topicPrefix + String(MQ_INTRO);
        String value = String(NRF_TYPE_LOCO) + NRF_SEPARATOR + node->locoAddr +
                       NRF_SEPARATOR + node->locoName + NRF_SEPARATOR +
                       VERSION + MQ_SEPARATOR + LOCO_FORMAT;
        write(topic, value);
        heartBeatKeys();
    }

    void reconnect()
    {
        if (millis() >= nextReconnectTime) {
            String brokerIP = settings.get("broker");
            int brokerPort = settings.getCachedInt("brokerport");
            mqtt.setServer(brokerIP.c_str(), brokerPort);
            if (mqtt.connect(node->locoName)) {
                Serial.println("[MQ] Connected");
                String topic = topicPrefix + "#";
                mqtt.subscribe(topic.c_str());
                introduce();
            } else {
                log("Failed to connect");
                nextReconnectTime = millis() + 2000;
            }
        }
    }

    void begin()
    {
        mqtt.setCallback(onMqttMessage);
        topicPrefix = String(MQ_PREFIX) + "/" + node->locoAddr + "/";
        heartbeatTopic = topicPrefix + String(MQ_HEARTBEAT_VALUES);
        nextReconnectTime = millis();
        heartbeatTimer.start();
        functions.begin();
    }

    void loop()
    {
        if (!mqtt.connected()) {
            reconnect();
        } else {
            if (heartbeatTimer.hasFiredOnce()) {
                heartbeat();
                heartbeatTimer.start(node->getHeartbeat());
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
    if (strcmp(action, MQ_INTRO_REQ) == 0) {
        mqttClient.introduce();
    } else if (strcmp(action, MQ_SET_THROTTLE) == 0) {
        // Throttle/speed
        int l = (length > 4) ? 4 : length;
        char throttle[5];
        strncpy(throttle, value, l);
        throttle[l] = '\0';
        mqttClient.node->setThrottle(atoi(throttle));
    } else if (strcmp(action, MQ_SET_DIRECTION) == 0) {
        // Direction
        for (int i = 0; i < 4; i++) {
            if ((strncmp(value, MQ_DIRECTIONS[i], length) == 0) ||
                (value[0] == '0' + i)) {
                mqttClient.node->setDirection(i);
                break;
            }
        }
    } else if (strcmp(action, MQ_GET_VALUE) == 0) {
        // Value, get state
        String key(payload, length);
        String value = mqttClient.node->getValue(key.c_str());
        String topic = mqttClient.topicPrefix + MQ_SET_VALUE + key;
        mqttClient.write(topic, value);
    } else if (strcmp(action, MQ_LIST_VALUE_REQ) == 0) {
        // Value, list all Keys (config and runtime)
        String topic = mqttClient.topicPrefix + MQ_LIST_VALUE_RES;
        String value = mqttClient.node->listValues();
        mqttClient.write(topic, value);
    } else if (strncmp(action, MQ_SET_VALUE, strlen(MQ_SET_VALUE)) == 0) {
        // Value, set state. Hast to be the last one, after "get" and "list"
        char *key = action + strlen(MQ_SET_VALUE);
        String value(payload, length);
        mqttClient.node->setValue(key, value.c_str());
    } else if (strcmp(action, MQ_GET_FUNCTION) == 0) {
        // Function, get state.
        String key(payload, length);
        uint8_t functionCode;
        if (mqttClient.functions.isValidFunction(key.c_str(), functionCode)) {
            String value = mqttClient.node->getFunction(functionCode) ? MQ_ON : MQ_OFF;
            String topic = mqttClient.topicPrefix + MQ_SET_FUNCTION + key;
            mqttClient.write(topic, value);
        }
    } else if (strcmp(action, MQ_LIST_FUNCTION_REQ) == 0) {
        // Function, list all function names.
        String topic = mqttClient.topicPrefix + MQ_LIST_FUNCTION_RES;
        String functionList = mqttClient.functions.getFunctionList();
        mqttClient.write(topic, functionList);
    } else if (strncmp(action, MQ_SET_FUNCTION_NAME, strlen(MQ_SET_FUNCTION_NAME)) == 0) {
        // Function name, set name for function ID
        action += strlen(MQ_SET_FUNCTION_NAME);
        uint8_t functionId = (uint8_t)atoi(action);
        String functionName(payload, length);
        mqttClient.functions.setFunction(functionId, functionName.c_str());
    } else if (strncmp(action, MQ_SET_FUNCTION, strlen(MQ_SET_FUNCTION)) == 0) {
        // Function, set state. Must be after "get" and "name"
        action += strlen(MQ_SET_FUNCTION);
        uint8_t functionCode;
        if (mqttClient.functions.isValidFunction(action, functionCode)) {
            if (length && strncmp(value, MQ_ON, length) == 0)
                mqttClient.node->setFunction(functionCode, true);
            else
                mqttClient.node->setFunction(functionCode, false);
        }
    } else if (strcmp(action, MQ_HEARTBEAT) == 0) {
        // Heartbeat request
        mqttClient.heartbeat();
    }
}
