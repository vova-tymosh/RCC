/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
#pragma once
#include <stdint.h>
#include "RCCNode.h"
#include "Settings.h"
#include "Protocol.h"

void (*reboot)(void) = 0;

class RccCli
{
private:
    static const uint8_t INPUT_LEN_MAX = 128;
    static const char END_MARKER = '\n';

    RCCNode *loco;

    bool processLine(char cmd[])
    {
        if (strlen(cmd) == 0)
            return false;
        char first = toupper(cmd[0]);
        if (strlen(cmd) > 0) {
            cmd++;
            switch (first) {
            case CMD_THROTTLE:
                return processThrottle(cmd);
            case CMD_DIRECTION:
                return processDirection(cmd);
            case CMD_SET_FUNCTION:
                return processFunction(cmd);
            case CMD_GET_FUNCTION:
                return processGetFunction(cmd);
            case CMD_CMD:
                return processCommand(cmd);
            case CMD_SET_VALUE:
                return processSet(cmd);
            case CMD_GET_VALUE:
                return processGet(cmd);
            case CMD_LIST_VALUE:
                return processList(cmd);
            case CMD_READ_FILE:
                return processRead(cmd);
            case CMD_WRITE_FILE:
                return processWrite(cmd);
            case CMD_APPEND_FILE:
                return processAppend(cmd);
            case CMD_SIZE_FILE:
                return processSize(cmd);
            case CMD_ERASE:
                return processErase(cmd);
            case CMD_REBOOT:
                reboot();
            }
        }
        return false;
    }

    bool processThrottle(char cmd[])
    {
        if (strlen(cmd) == 0)
            return false;
        uint8_t value = (uint8_t)atoi(cmd);
        loco->setThrottle(value);
        return true;
    }

    bool processDirection(char cmd[])
    {
        if (strlen(cmd) == 0)
            return false;
        uint8_t value = (uint8_t)atoi(cmd);
        loco->setDirection(value);
        return true;
    }

    bool processFunction(char cmd[])
    {
        if (strlen(cmd) < 2)
            return false;
        uint8_t function = (uint8_t)atoi(cmd + 1);
        loco->setFunction(function, cmd[0] == '1');
        return true;
    }

    bool processGetFunction(char cmd[])
    {
        if (strlen(cmd) < 1)
            return false;
        uint8_t function = (uint8_t)atoi(cmd);
        bool activate = loco->getFunction(function);
        Serial.print("F");
        Serial.print(cmd);
        Serial.print(": ");
        Serial.println(activate ? "ON" : "OFF");
        return true;
    }

    bool processCommand(char cmd[])
    {
        if (strlen(cmd) == 0)
            return false;
        loco->onCommand(toupper(cmd[0]), cmd + 1, strlen(cmd) - 1);
        return true;
    }

    bool processSet(char cmd[])
    {
        if (strlen(cmd) < 2)
            return false;
        char *separator = strchr(cmd, ':');
        if (separator == NULL)
            return false;
        *separator = '\0';
        char *key = cmd;
        char *value = ++separator;
        loco->setValue(key, value);
        Serial.print(key);
        Serial.print(":");
        Serial.println(value);
        return true;
    }

    bool processGet(char cmd[])
    {
        if (strlen(cmd) < 1)
            return false;
        char *key = cmd;
        String value = loco->getValue(key);
        Serial.print(key);
        Serial.print(":");
        Serial.println(value);
        return true;
    }

    bool processList(char cmd[])
    {
        Serial.println(loco->listValues());
        return true;
    }

    bool processRead(char cmd[])
    {
        if (strlen(cmd) < 1)
            return false;
        char *filename = cmd;
        char value[INPUT_LEN_MAX];
        storage.read(filename, value, sizeof(value));
        Serial.print(filename);
        Serial.print(":");
        Serial.println(value);
        return true;
    }

    bool processWrite(char cmd[])
    {
        if (strlen(cmd) < 2)
            return false;
        char *separator = strchr(cmd, ':');
        if (separator == NULL)
            return false;
        *separator = '\0';
        char *filename = cmd;
        char *value = ++separator;
        storage.write(filename, value, strlen(value) + 1);
        Serial.print("Write: ");
        Serial.println(filename);
        return true;
    }

    bool processAppend(char cmd[])
    {
        if (strlen(cmd) < 2)
            return false;
        char *separator = strchr(cmd, ':');
        if (separator == NULL)
            return false;
        *separator = '\0';
        char *filename = cmd;
        char *sizeStr = ++separator;
        size_t size = atoi(sizeStr);
        const int bufferSize = 4096;
        if (size > bufferSize)
            size = bufferSize;
        char *buffer = (char *)malloc(size);
        if (buffer == NULL) {
            Serial.println("Error: Out of memory");
            return false;
        }
        size_t r = Serial.readBytes(buffer, size);
        storage.append(filename, buffer, r);
        Serial.print("Append bytes: ");
        Serial.println(r);
        free(buffer);
        return true;
    }

    bool processSize(char cmd[])
    {
        if (strlen(cmd) < 1)
            return false;
        char *filename = cmd;
        size_t size = storage.size(filename);
        Serial.print("Size of ");
        Serial.print(filename);
        Serial.print(": ");
        Serial.println(size);
        return true;
    }

    bool processErase(char cmd[])
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
    RccCli(RCCNode *loco) : loco(loco) {}

    void loop()
    {
        char line[INPUT_LEN_MAX];
        if (getLine(line)) {
            bool result = processLine(line);
            if (!result) {
                Serial.print("Error, can't parse: ");
                Serial.println(line);
            }
        }
    }
};
