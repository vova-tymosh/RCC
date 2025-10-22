/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
#pragma once
#include "Storage.h"

class Functions
{
private:
    static const char *FUNCTION_NAMES_FILE;
    static const int MAX_FUNCTIONS = 32;
    static const int MAX_NAME_LENGTH = 16;

    struct FunctionMapping {
        uint8_t id;
        char name[MAX_NAME_LENGTH];
    };

    FunctionMapping mappings[MAX_FUNCTIONS];
    int mappingCount;

    void parseFile(const char *data, size_t size);
    void saveToFile();
    int findMappingByName(const char *name);
    int findMappingById(uint8_t id);

public:
    Functions();

    void begin();

    // Convert function name to ID, returns -1 if not found
    int nameToId(const char *name);

    // Convert function ID to name, returns nullptr if not found
    const char *idToName(uint8_t id);

    // Add or update function mapping
    void setFunction(uint8_t id, const char *name);

    // Check if a string is a valid function identifier (name or number)
    bool isValidFunction(const char *identifier, uint8_t &outId);

    // Get function count
    int getCount() const
    {
        return mappingCount;
    }

    // Get mapping by index
    const FunctionMapping *getMapping(int index) const;

    // Get formatted function list as string (id,name,id,name,...)
    String getFunctionList() const;
};