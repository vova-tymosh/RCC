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
#include <ESPmDNS.h>
#include "Timer.h"
#include "Settings.h"
#include "esp32/TransportClient.h"

//
// https://www.jmri.org/help/en/package/jmri/jmrit/withrottle/Protocol.shtml
//

#if RCC_DEBUG >= 2
#define log(msg)                                                                                   \
    {                                                                                              \
        Serial.print("[WT] ");                                                                     \
        Serial.println(msg);                                                                       \
    };
#else
#define log(msg)
#endif

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

class WiThrottleClient : public TransportClient
{
protected:
    const char *propertySeparator = "<;>";
    const int heartbeatTimeout = 10;

    const int port = 44444;
    const char *mdnsName = "withrottle";
    WiFiServer server;
    WiFiClient conn;
    LineReceiver lr;
    String locoAddr;

public:
    WiThrottleClient() : server(port) {}

    void reply(String msg)
    {
        log(String("> ") + msg);
        conn.println(msg);
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
            reply("*" + String(heartbeatTimeout));
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

    void processAddLoco(char throttleId, char *key, char *value)
    {
        String prefix = String("M") + throttleId + "A" + key + "<;>";
        reply(String("M") + throttleId + "+" + key + "<;>");

        // Build function names list for functions 0-9
        String functionList = String("M") + throttleId + "L" + key + "<;>";
        for (int i = 0; i <= 9; i++) {
            functionList += "]\\[";
            const char *functionName = node->functions.idToName(i);
            if (functionName != NULL) {
                functionList += functionName;
            }
        }
        reply(functionList);

        for (int i = 0; i <= 9; i++) {
            bool state = node->getFunction(i);
            char stateChar = (state) ? '1' : '0';
            reply(prefix + "F" + String(stateChar) + i);
        }
        reply(prefix + "V" + getThrottle());
        reply(prefix + "R" + node->getDirection());
        reply(prefix + "s1");
    }

    void processDelLoco(char throttleId, char *key, char *value)
    {
        reply(String("M") + throttleId + "-" + key + "<;>");
    }

    void processAction(char throttleId, char *key, char *value)
    {
        // key = loco addr, or *, ignore it
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
            node->setDirection(atoi(value));
            break;
        case 'q':
            if (*value == 'V') {
                int v = getThrottle();
                reply(String("M") + throttleId + "A" + locoAddr + "<;>V" + v);
            } else if (*value == 'R') {
                int d = node->getDirection();
                reply(String("M") + throttleId + "A" + locoAddr + "<;>R" + d);
            }
            break;
        }
    }

    void processFunction(char throttleId, char *value)
    {
        if (strlen(value) < 2)
            return;
        bool press = !(value[0] == '0');
        int functId = atoi(value + 1);
        bool state = node->getFunction(functId);
        if (press) {
            state = !state;
            node->setFunction(functId, state);
        }
        char stateChar = (state) ? '1' : '0';
        reply(String("M") + throttleId + "A" + locoAddr + "<;>" + "F" + stateChar +
              String(functId));
    }

    int getThrottle()
    {
        int v = node->getThrottle();
        v = map(v, 0, 100, 0, 126);
        return v;
    }

    void setThrottle(char *value)
    {
        int v = atoi(value);
        v = map(v, 0, 126, 0, 100);
        node->setThrottle(v);
    }

    void connect()
    {
        log("Connected");
        conn.flush();
        conn.setTimeout(500);

        uint addr = node->locoAddr;
        char addrType = (addr < 127) ? 'S' : 'L';
        locoAddr = String(addrType) + addr;

        reply("VN2.0");
        reply(String("RL1]\\[") + node->locoName + "}|{" + addr + "}|{" + addrType);
        reply("PPA1");
        reply("");
        reply("*" + String(heartbeatTimeout));
    }

    void begin()
    {
        MDNS.begin(node->locoName);
        server.begin();
        MDNS.addService(mdnsName, "tcp", port);
        Serial.println("[WT] Started");
    }

    void loop()
    {
        if (!conn) {
            conn = server.available();
            if (conn)
                connect();
        } else {
            if (conn.available()) {
                char *line = lr.getLine(conn.read());
                if (line)
                    processLine(line);
            }
        }
    }
};
