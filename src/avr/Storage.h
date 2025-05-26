#pragma once

#define F_WRITE_MODE (1)
#define BUILD_FILE() File()

void beginPhy();
bool cleanPhy();

class File
{
public:
    static const int NAME_LEN = 12;
    struct FileRecord {
        char filename[NAME_LEN];
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

    File() : isOpen(false), offset(0) {}

    File(const File& file) : isOpen(file.isOpen), offset(file.offset)
    {
        memcpy(&f, &file.f, sizeof(f));
    }

    bool format()
    {
        return true;
    }

    char *name()
    {
        return f.filename;
    }

    void close(void)
    {
        isOpen = false;
    }

    operator bool (void)
    {
        return isOpen;
    }

    bool begin();

    File openNextFile();

    bool exists(char const *filepath);

    bool remove(char const *filepath);    

    File open(char const *filename, uint8_t mode = 0);

    size_t write(uint8_t const *buffer, size_t size);

    size_t read(uint8_t *buffer, size_t size);

    bool seek(uint32_t pos);
};

extern File fs;


