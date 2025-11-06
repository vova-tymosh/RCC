/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
#include "Functions.h"
#include "Settings.h"
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>

Functions::Functions() : mappingCount(0)
{
    memset(mappings, 0, sizeof(mappings));
}

void Functions::begin()
{
    mappingCount = 0;

    // Read function mappings from settings
    String data = settings.get("functions");
    if (data.length() > 0) {
        parseFile(data.c_str(), data.length());
    }
}

void Functions::parseFile(const char *data, size_t size)
{
    if (size == 0)
        return;

    char *buffer = (char *)malloc(size + 1);
    if (buffer == NULL)
        return;

    memcpy(buffer, data, size);
    buffer[size] = '\0';

    char *token = strtok(buffer, ",");

    while (token != NULL && mappingCount < MAX_FUNCTIONS) {
        // Parse ID
        uint8_t id = (uint8_t)atoi(token);
        token = strtok(NULL, ",");

        if (token != NULL) {
            // Parse name
            mappings[mappingCount].id = id;
            strncpy(mappings[mappingCount].name, token, MAX_NAME_LENGTH - 1);
            mappings[mappingCount].name[MAX_NAME_LENGTH - 1] = '\0';
            mappingCount++;

            token = strtok(NULL, ",");
        } else {
            break;
        }
    }

    free(buffer);
}

void Functions::saveToFile()
{
    if (mappingCount == 0) {
        settings.set("functions", "");
        return;
    }

    String buffer;
    buffer.reserve(mappingCount * 20); // Estimate space needed

    for (int i = 0; i < mappingCount; i++) {
        if (i > 0) {
            buffer += ",";
        }
        buffer += String(mappings[i].id);
        buffer += ",";
        buffer += mappings[i].name;
    }

    settings.set("functions", buffer);
}

int Functions::findMappingByName(const char *name)
{
    for (int i = 0; i < mappingCount; i++) {
        if (strcmp(mappings[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int Functions::findMappingById(uint8_t id)
{
    for (int i = 0; i < mappingCount; i++) {
        if (mappings[i].id == id) {
            return i;
        }
    }
    return -1;
}

int Functions::nameToId(const char *name)
{
    int index = findMappingByName(name);
    return (index >= 0) ? mappings[index].id : -1;
}

const char *Functions::idToName(uint8_t id)
{
    int index = findMappingById(id);
    return (index >= 0) ? mappings[index].name : NULL;
}

void Functions::setFunction(uint8_t id, const char *name)
{
    if (name == NULL || strlen(name) == 0)
        return;

    // Check if ID already exists
    int existingIndex = findMappingById(id);

    if (existingIndex >= 0) {
        // Update existing mapping
        strncpy(mappings[existingIndex].name, name, MAX_NAME_LENGTH - 1);
        mappings[existingIndex].name[MAX_NAME_LENGTH - 1] = '\0';
    } else if (mappingCount < MAX_FUNCTIONS) {
        // Add new mapping
        mappings[mappingCount].id = id;
        strncpy(mappings[mappingCount].name, name, MAX_NAME_LENGTH - 1);
        mappings[mappingCount].name[MAX_NAME_LENGTH - 1] = '\0';
        mappingCount++;
    }
    saveToFile();
}

bool Functions::isValidFunction(const char *identifier, uint8_t &outId)
{
    if (identifier == NULL || strlen(identifier) == 0) {
        return false;
    }

    // Check if it's a number
    char *endPtr;
    long id = strtol(identifier, &endPtr, 10);

    if (*endPtr == '\0' && id >= 0 && id <= 255) {
        // It's a valid number
        outId = (uint8_t)id;
        return true;
    }

    // Check if it's a function name
    int mappedId = nameToId(identifier);
    if (mappedId >= 0) {
        outId = (uint8_t)mappedId;
        return true;
    }

    return false;
}

const Functions::FunctionMapping *Functions::getMapping(int index) const
{
    if (index >= 0 && index < mappingCount) {
        return &mappings[index];
    }
    return NULL;
}

String Functions::getFunctionList() const
{
    String result;
    result.reserve(mappingCount * 20); // Estimate space needed

    for (int i = 0; i < mappingCount; i++) {
        if (i > 0) {
            result += ",";
        }
        result += String(mappings[i].id);
        result += ",";
        result += mappings[i].name;
    }

    return result;
}