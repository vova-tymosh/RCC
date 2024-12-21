#pragma once
#include <WiFiClient.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include "Timer.h"
#include "RCCLocoBase.h"

//
// https://www.jmri.org/help/en/package/jmri/jmrit/withrottle/Protocol.shtml
//



class LineReceiver
{
private:
    static const int MAX_LINE = 256;
    char line[MAX_LINE];
    int linePointer = 0;
public:
    char *getLine(char c)
    {        
        if (c == '\n' || c == '\r') {
            line[linePointer] = '\0';
            linePointer = 0;
            return line;
        } else if (c > 0) {
            if (linePointer >= MAX_LINE) {
                Serial.println("Error, line is too long");
                linePointer = 0;
            }
            line[linePointer++] = c;
        }
        return NULL;
    }
};

class WiThrottleClient
{
protected:
    WiFiClient conn;
    LineReceiver lr;
    const char *locoAddr = "S3";
    const char *propertySeparator = "<;>";
    const int heartbeatTimeout = 10;
    RCCLocoBase *loco;

public:

    void log(String msg)
    {
        Serial.println(msg);
    }

    void reply(String msg)
    {
        log(String("> ")+msg);
        conn.println(msg);
    }

    void begin(WiFiClient _conn, RCCLocoBase *_loco)
    {
        conn = _conn;
        loco = _loco;

        log("WiThrottle Start");
        conn.flush();
        conn.setTimeout(500);

        reply("VN2.0");
        //TODOD - loco name/addr     
        reply("RL1]\\[RGS 3}|{3}|{S");
        reply("PPA1");
        reply("");
        reply("*"+String(heartbeatTimeout));
    }

    void loop()
    {
        if (conn.available()) {
            char *line = lr.getLine(conn.read());
            if (line)
                processLine(line);
        }
    }

    void processLine(char *line)
    {
        log(String("< ") + line);
        if (strlen(line) < 1)
            return;
        char first = *line;
        line++;
        switch (first) {
        case 'N':
            reply("*"+String(heartbeatTimeout));
            break;
        case 'M':
            processMultiThrottle(line);
            break;
        }
    }

    void processMultiThrottle(char *line)
    {
        if (strlen(line) < 3)
            return;
        char *separator = strstr(line, propertySeparator);
        if (separator == NULL)
            return;
        char throttleId = *line;
        line++;
        char command = *line;
        line++;
        *separator = '\0';
        separator += strlen(propertySeparator);
        char *key = line;
        char *value = separator;

        switch (command) {
        case '+':
            processAddLoco(throttleId, key, value);
            break;
        case '-':
            processDelLoco(throttleId, key, value);
            break;
        case 'A':
            processAction(throttleId, key, value);
            break;
        }
    }

    void processAddLoco(char throttleId, char *key, char *value) {
        String prefix = String("M")+throttleId+"A"+key+"<;>";
        reply(String("M")+throttleId+"+"+key+"<;>");
        for(int i = 0; i < 29; i++){
            bool state = loco->getFunction(i);
            char stateChar = (state) ? '1' : '0';
            reply(prefix+"F"+String(state)+i);
        }
        reply(prefix + "V" + getThrottle());
        reply(prefix + "R" + loco->getDirection());
        reply(prefix + "s1");
    }

    void processDelLoco(char throttleId, char *key, char *value) {
        reply(String("M")+throttleId+"-"+key+"<;>");
    }

    void processAction(char throttleId, char *key, char* value)
    {        
        //key = loco addr, or *, ignore it
        char actionId = *value;
        value++;
        if (strlen(value) < 1)
            return;
        switch (actionId) {
        case 'F':
            processFunction(throttleId, value);
            break;
        case 'V':
            setThrottle(value);
            break;
        case 'R':
            loco->setDirection(atoi(value));
            break;
        case 'q':
            if (*value == 'V') {
                int v = getThrottle();
                reply(String("M")+throttleId+"A"+locoAddr+"<;>V"+v);
            } else if (*value == 'R') {
                int d = loco->getDirection();
                reply(String("M")+throttleId+"A"+locoAddr+"<;>R"+d);                
            }
            break;
        }
    }

    void processFunction(char throttleId, char* value)
    {
        if (strlen(value) < 2)
            return;
        bool press = !(value[0] == '0');
        int functId = atoi(value + 1);
        bool state = loco->getFunction(functId);
        if (press) {
            state = !state;
            loco->setFunction(state, functId);
        }
        char stateChar = (state) ? '1' : '0';
        reply(String("M")+throttleId+"A"+locoAddr+"<;>"+"F"+stateChar+String(functId));
    }

    int getThrottle()
    {
        int v = loco->getThrottle();
        v = map(v, 0, 100, 0, 126);
        return v;
    }

    void setThrottle(char *value)
    {
        int v = atoi(value);
        v = map(v, 0, 126, 0, 100);
        loco->setThrottle(v);
    }
};




class TransportWT
{
private:
    const int port = 44444;
    const char *mdnsName = "withrottle";
    //TODO proper name
    const char *hostname = "RCC_Loco";
    WiFiServer server;
    WiFiClient client;
    WiThrottleClient wtClient;
    RCCLocoBase *loco;

public:
    TransportWT(RCCLocoBase *loco) : server(port), loco(loco) {}

    void begin() {
        MDNS.begin(hostname);
        server.begin();
        MDNS.addService(mdnsName, "tcp", port);
    }

    void loop() {
        if (!client) {
            client = server.available();
            if (client)
                wtClient.begin(client, loco);
        } else {
            wtClient.loop();
        }
    }
};
