/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
#pragma once
#include "Platform.h"

#define RCC_FILE_WRITE 1
#define RCC_FILE_APPEND 1

#define BUILD_FILE() File()

void beginPhy();
bool cleanPhy();

class File
{
public:
    struct FileRecord {
        char filename[FILENAME_LEN];
        uint16_t addr;
        uint8_t size;
    };

private:
    struct FileRecord f;
    uint16_t offset;
    bool isOpen;

    bool createRecord(const char *filename, size_t size, FileRecord *record);

    bool getRecord(const char *filename, FileRecord *record);

public:
    // FS level functions
    bool begin(bool format = true);

    void clear();

    File openNextFile();

    bool exists(char const *filepath);

    bool mkdir(const char *path)
    {
        return true;
    }

    // File level functions
    File() : isOpen(false), offset(0) {}

    File(const File &file) : isOpen(file.isOpen), offset(file.offset)
    {
        memcpy(&f, &file.f, sizeof(f));
    }

    char *name()
    {
        return f.filename;
    }

    uint8_t size()
    {
        return f.size;
    }

    void close(void)
    {
        isOpen = false;
    }

    operator bool(void)
    {
        return isOpen;
    }

    File open(char const *filename, uint8_t mode = 0);

    size_t write(uint8_t const *buffer, size_t size);

    size_t read(uint8_t *buffer, size_t size);

    bool seek(uint16_t pos);
};

extern File LittleFS;
