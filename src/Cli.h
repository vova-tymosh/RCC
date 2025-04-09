#pragma once
#include <stdint.h>
#include "RCCLocoBase.h"
#include "Settings.h"

void (*reboot)(void) = 0;

class RccCli
{
private:
    static const uint8_t INPUT_LEN_MAX = 32;
    static const char END_MARKER = '\n';

    static const char CMD_SPEED = 'S';
    static const char CMD_FUNCTION = 'F';
    static const char CMD_CMD = 'C';
    static const char CMD_PUT = 'P';
    static const char CMD_GET = 'G';
    static const char CMD_LIST = 'L';
    static const char CMD_ERASE = 'E';
    static const char CMD_REBOOT = '!';

    RCCLocoBase *loco;

    bool processLine(char cmd[])
    {
        if (strlen(cmd) == 0)
            return false;
        char first = toupper(cmd[0]);
        if (strlen(cmd) > 0) {
            cmd++;
            if (first == CMD_SPEED)
                return processSpeed(cmd);
            else if (first == CMD_FUNCTION)
                return processFunction(cmd);
            else if (first == CMD_CMD)
                return processCommand(cmd);
            else if (first == CMD_PUT)
                return processPut(cmd);
            else if (first == CMD_GET)
                return processGet(cmd);
            else if (first == CMD_LIST)
                return processList(cmd);
            else if (first == CMD_ERASE)
                return processClear(cmd);
            else if (first == CMD_REBOOT)
                reboot();
        }
        return false;
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
        loco->onThrottle(dir, (uint8_t)speed);
        return true;
    }

    bool processFunction(char cmd[])
    {
        if (strlen(cmd) < 2)
            return false;
        uint8_t function = (uint8_t)atoi(cmd + 1);
        loco->onFunction(function, cmd[0] == '1');
        return true;
    }

    bool processCommand(char cmd[])
    {
        if (strlen(cmd) == 0)
            return false;
        loco->onCommand(cmd[0], 0);
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
        loco->putValue(key, value);
        Serial.println(String(key) + ":" + value);
        return true;
    }

    bool processGet(char cmd[])
    {
        if (strlen(cmd) < 1)
            return false;
        char *key = cmd;
        String value = loco->getValue(key);
        Serial.println(String(key) + ":" + value);
        return true;
    }

    bool processList(char cmd[])
    {
        Serial.println(loco->listValues());
        return true;
    }

    bool processClear(char cmd[])
    {
        storage.clear();
        Serial.println("Clear");
        return true;
    }

    int getLine(char *line)
    {
        int i = 0;
        memset(line, 0, INPUT_LEN_MAX);
        while (Serial.available() > 0) {
            char rc = Serial.read();
            if (rc == END_MARKER)
                break;
            else if (i < INPUT_LEN_MAX - 1) {
                line[i] = rc;
                i++;
            }
        }
        return i > 0;
    }

public:
    RccCli(RCCLocoBase *loco) : loco(loco) {}

    void loop()
    {
        char line[INPUT_LEN_MAX];
        if (getLine(line)) {
            bool result = processLine(line);
            if (!result) {
                Serial.print("Error, can't parse");
            }
        }
    }
};
