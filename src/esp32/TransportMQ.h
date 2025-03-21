#pragma once
#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include "Timer.h"
#include "Settings.h"


//
// https://www.jmri.org/help/en/html/hardware/mqtt/index.shtml
//


void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i=0;i<length;i++) {
      Serial.print((char)payload[i]);
    }
    Serial.println();
}
  
class MqttClient
{
private:
    const char *BROCKER = "192.168.20.61";
    const int PORT = 1883;

    WiFiClient conn;
    PubSubClient mqtt;

    String locoName;
    String locoAddr;
    Timer reconnectTimer;
    Timer heartbeatTimer;  
  
public:

    
    MqttClient(RCCLocoBase *loco): mqtt(conn) {};


    void reconnect() {
        // Loop until we're reconnected
        if (!mqtt.connected()) {
            Serial.print("Attempting MQTT connection...");
            // Attempt to connect
            if (mqtt.connect("arduinoClient")) {
                Serial.println("connected");
                // Once connected, publish an announcement...
                mqtt.publish("outTopic","hello world");
                // ... and resubscribe
                mqtt.subscribe("inTopic");
            } else {
                Serial.print("failed, rc=");
                Serial.print(mqtt.state());
                Serial.println(" try again in 5 seconds");
                // Wait 5 seconds before retrying
                delay(5000);
            }
        }
    }

    void begin()
    {
        mqtt.setServer("192.168.20.61", 1883);
        mqtt.setCallback(callback);
    }

    void loop()
    {
        if (!mqtt.connected()) {
            reconnect();
        }
        mqtt.loop();
    }
};
  























/*
RCCLocoBase *localLocoPointer;

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

void onMessage(char* topic, byte* payload, unsigned int length)
{
    // const int MAX_LEN = 16;
    // char value[MAX_LEN];
    // if (length > MAX_LEN)
    //     length = MAX_LEN;
    // strncpy(value, (char *)payload, length);
    // value[length] = '\0';

    // char *action = getAction(topic);
    // if (strcmp(action, "throttle") == 0) {
    //     localLocoPointer->setThrottle(atoi(value));
    // } else if (strcmp(action, "direction") == 0) {
    //     if (strcmp(value, "FORWARD") == 0) {
    //         localLocoPointer->setDirection(1);
    //     } else if (strcmp(value, "REVERSE") == 0) {
    //         localLocoPointer->setDirection(0);
    //     } else {
    //         localLocoPointer->setDirection(1);
    //         localLocoPointer->setThrottle(0);
    //     }
    // } else if (strncmp(action, "function/", strlen("function/")) == 0) {
    //     action += strlen("function/");
    //     int code = atoi(action);
    //     if (strcmp(value, "ON") == 0)
    //         localLocoPointer->setFunction(code, true);
    //     else 
    //         localLocoPointer->setFunction(code, false);
    // }


    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i=0;i<length;i++) {
      Serial.print((char)payload[i]);
    }
    Serial.println();
}

class MQTTClient
{
protected:
    const char *BROCKER = "192.168.20.61";
    const int PORT = 1883;

    PubSubClient client;

    String locoName;
    String locoAddr;
    Timer reconnectTimer;
    Timer heartbeatTimer;

    // char topic[20];
    // char payload[256];
public:
    MQTTClient(RCCLocoBase *_loco)
    {
        localLocoPointer = _loco;
    }

    void log(String msg)
    {
        if (localLocoPointer->debugLevel > 1)
            Serial.println(msg);
    }

    void authorize()
    {
        char topic[32];
        snprintf(topic, sizeof(topic), "cab/%s/heartbeat_fields", locoAddr.c_str());
        log(String("> ") + topic + " " + FIELDS); //locoName + " "
        // client.publish(topic, FIELDS, true);
    }

    void heartbeat()
    {
        log(">>>> ");
        
        // snprintf(payload, sizeof(payload), "zu zu zu 22");

        // snprintf(topic, sizeof(topic), "cab/%s/heartbeat", locoAddr.c_str());
        // strcpy(payload, "20 30 245 123123");

        // LocoState *s = &localLocoPointer->state;
        // snprintf(payload, sizeof(payload), "%s %s %s %s %s %s %s %s %s %s %s", 
        //     s->tick, s->distance, s->bitstate, s->speed, s->lost, 
        //     s->throttle, s->throttle_out, s->battery, s->temperature, s->psi, s->water);


        // log(String("> ") + topic + " " + payload);
        // client.publish("cab/3/heartbeat", "20 30 245 123123");
    }

    void begin(WiFiClient conn)
    {
        client.setClient(conn);
        client.setServer(BROCKER, PORT);
        client.setCallback(onMessage);

        if (client.connect("rcc")) {
            log("[MQ] Connected");
        } else {
            log("[MQ] Failed to connect");
        }

        locoName = settings.get("loconame");
        locoAddr = settings.get("locoaddr");
        String topic = "cab/" + locoAddr + "/#";
        client.subscribe(topic.c_str());        
    }

    void loop()
    {
        client.loop();
        if (!client.connected()) {
            if (reconnectTimer.hasFiredOnce())
            //locoName.c_str()
                if (client.connect("rcc222")) {
                    String topic = "cab/" + locoAddr + "/#";
                    client.subscribe(topic.c_str());
                    // authorize();
                    heartbeatTimer.start(5000);
                    log("[MQ] Connected");
                } else {
                    log("[MQ] Failed to connect");
                    reconnectTimer.start(1000);
                }
        } else {
            client.loop();
        }

        if (heartbeatTimer.hasFired()) {
            heartbeatTimer.start(1000);
            // heartbeat();
        }
    }
};

*/