#pragma once

#define F_WRITE_MODE (1)

void beginPhy();

class File
{
    struct FileRecord {
        char filename[16];
        uint32_t offset;
        uint32_t size;
    } f;

    uint32_t createFile(const char *filename, size_t size);

    bool getFile(const char *filename, FileRecord *record);

public:

    File() {}

    File(char const *filename, uint8_t mode = 0);

    bool begin()
    {
        return true;
    }

    bool format()
    {
        return true;
    }

    File openNextFile();

    bool exists(char const *filepath);

    bool remove(char const *filepath);    

    File open(char const *filename, uint8_t mode = 0);

    char *name()
    {
        return f.filename;
    }

    size_t write(uint8_t const *buf, size_t size);

    int read(uint8_t *buf, size_t size);

    bool seek(uint32_t pos);

    void close(void) {}

    operator bool (void);

};

extern File fs;


