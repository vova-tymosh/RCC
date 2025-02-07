#pragma once
#include <stdint.h>
#include "RCCLocoBase.h"
#include "Settings.h"

class Cli
{
private:
    static const uint8_t INPUT_LEN_MAX = 12;
    static const char END_MARKER = '\n';

    static const char CMD_SPEED = 'S';
    static const char CMD_FUNCTION = 'F';
    static const char CMD_READ = 'R';
    static const char CMD_PUT = 'P';
    static const char CMD_GET = 'G';
    static const char CMD_CLEAR = 'C';

    RCCLocoBase *loco;

    bool processLine(char cmd[])
    {
        if (strlen(cmd) == 0)
            return false;
        char first = toupper(cmd[0]);
        if (strlen(cmd) > 1) {
            cmd++;
            if (first == CMD_READ)
                return processExe(cmd);
            else if (first == CMD_SPEED)
                return processSpeed(cmd);
            else if (first == CMD_FUNCTION)
                return processFunction(cmd);
            else if (first == CMD_PUT)
                return processPut(cmd);
            else if (first == CMD_GET)
                return processGet(cmd);
            else if (first == CMD_CLEAR)
                return processClear(cmd);
        }
        return false;
    }

    bool processExe(char cmd[]) 
    {
        if (strlen(cmd) == 0)
            return false;
        onExe(cmd[0]);
        return true;
    }

    bool processSpeed(char cmd[]) 
    {
        if (strlen(cmd) == 0)
            return false;
        char dir_text[2] = {cmd[0], '\0'};
        uint8_t dir = (uint8_t)atoi(dir_text);
        int speed = 0;
        if (strlen(cmd) > 1) {
            speed = atoi(cmd + 1);
            if (speed > 100)
                return false;
        }
        onThrottle(dir, (uint8_t)speed);
        return true;
    }

    bool processFunction(char cmd[]) 
    {
        if (strlen(cmd) < 2)
            return false;
        uint8_t function = (uint8_t)atoi(cmd + 1);
        onFunction(function, cmd[0] == '1');
        return true;
    }

    bool processPut(char cmd[]) 
    {
        if (strlen(cmd) < 2)
            return false;
        char *separator = strchr(cmd, ':');
        if (separator == NULL)
            return false;
        *separator = '\0';
        char *key = cmd;
        char *value = ++separator;
        onPut(key, value);
        return true;
    }

    bool processGet(char cmd[]) 
    {
        if (strlen(cmd) < 1)
            return false;
        char *key = cmd;
        onGet(key);
        return true;
    }

    bool processClear(char cmd[]) 
    {
        onClear();
        return true;
    }

public:
    Cli(RCCLocoBase *loco) : loco(loco) {}

    virtual void onExe(uint8_t code) {}

    virtual void onThrottle(uint8_t direction, uint8_t throttle)
    {
        loco->onThrottle(direction, throttle);
    }

    virtual void onFunction(uint8_t code, bool value)
    {
        loco->onFunction(code, value);
    }

    void onPut(char *key, char *value)
    {
        settings.put(key, value);
        Serial.println(String("Put=")+key+":"+value);
    }

    void onGet(char *key)
    {
        String value = settings.get(key);
        Serial.println(String("Get=")+key+":"+value);
    }

    void onClear()
    {
        storage.clear();
        Serial.println(String("Clear"));
    }

    void loop() 
    {
        char line[INPUT_LEN_MAX];
        int i = 0;
        while (Serial.available() > 0) {
            char rc = Serial.read();
            if (rc == END_MARKER)
                break;
            else if (i < INPUT_LEN_MAX) {
                line[i] = rc;
                i++;
            }
        }
        if (i) {
            line[i] = '\0';
            bool result = processLine(line);
            Serial.print(line);
            Serial.println(result ? " ok" : " error");
        }
    }
};

