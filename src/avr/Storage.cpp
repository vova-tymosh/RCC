#if defined(ARDUINO_AVR_LEONARDO)

/*
SPI flash storage for nRF52
Important:
    The flash writes with 4 bytes alignment, so the size of the file will be
rounded up to the next multiple of 4 Also FileRecords has to be aligned to 4
bytes


*/

#include <stdint.h>
#include <Arduino.h>
#include "avr/Storage.h"
#include "EEPROM.h"


const uint16_t maxFiles = 8;

int eeprom_read(int addr, uint8_t *data, size_t size)
{
    for (int i = 0; i < size; i++) {
        data[i] = EEPROM.read(addr + i);
    }
    return size;
}
int eeprom_write(int addr, uint8_t *data, size_t size)
{
    for (int i = 0; i < size; i++) {
        EEPROM.write(addr + i, data[i]);
    }
    return size;
}

#define phyBegin() ;
#define phyErase()                                                             \
    {                                                                          \
        for (int i = 0; i < EEPROM.length(); i++)                              \
            EEPROM.write(i, 0);                                                \
    };
#define phySize (EEPROM.length())




struct MasterRecord {
    uint32_t count;
    uint32_t end;
};

MasterRecord mr;

uint32_t Storage::createFile(const char *filename, size_t size)
{
    if (mr.end + size > phySize) {
        Serial.println("[FS] Storage full");
        return 0;
    }
    FileRecord f;
    strncpy(f.filename, filename, sizeof(f.filename));
    f.offset = mr.end;
    f.size = size;
    uint32_t recordAt = mr.count * sizeof(FileRecord) + sizeof(MasterRecord);
    eeprom_write(recordAt, (const uint8_t *)&f, sizeof(f));
    mr.count++;
    mr.end += size;
    eeprom_write(0, (const uint8_t *)&mr, sizeof(mr));
    return f.offset;
}

bool Storage::getFile(const char *filename, FileRecord *record)
{
    FileRecord f;
    uint32_t offset = sizeof(mr);
    for (int i = 0; i < mr.count; i++) {
        eeprom_read(offset + i * sizeof(f), (uint8_t *)&f, sizeof(f));
        if (strcmp(f.filename, filename) == 0) {
            memcpy(record, &f, sizeof(f));
            return true;
        }
    }
    return false;
}

// int Storage::read(const char *filename, void *buffer, size_t size,
//                   size_t offset)
// {
//     FileRecord f;
//     if (getFile(filename, &f)) {
//         if (offset >= f.size)
//             return 0;
//         if (offset + size > f.size)
//             size -= offset + size - f.size;
//         return phyRead(f.offset + offset, (uint8_t *)buffer, size);
//     }
//     return 0;
// }

// int Storage::write(const char *filename, void *buffer, size_t size,
//                    size_t offset)
// {
//     FileRecord f;
//     if (getFile(filename, &f)) {
//         if (offset >= f.size)
//             return 0;
//         if (offset + size > f.size)
//             size -= offset + size - f.size;
//         return phyWrite(f.offset + offset, (const uint8_t *)buffer, size);
//     } else {
//         uint32_t fileOffset = createFile(filename, size);
//         if (fileOffset)
//             return phyWrite(fileOffset, (const uint8_t *)buffer, size);
//     }
//     Serial.print("[FS] Failed to write file: ");
//     Serial.println(filename);
//     return 0;
// }

bool Storage::exists(const char *filename)
{
    FileRecord f;
    return getFile(filename, &f);
}

bool Storage::allocate(const char *filename, size_t size)
{
    return (createFile(filename, size) > 0);
}



void beginPhy()
{
}

File::File()
{

}

File::File(char const *filename, uint8_t mode = 0)
{

}

File File::openNextFile(uint8_t mode = 0)
{
    File f;
    return f;
}

bool File::exists(char const *filepath)
{
    return true;
}

bool File::remove(char const *filepath)
{
    return true;
}

File File::open(char const *filename, uint8_t mode = 0)
{
    File f;
    return f;
}

size_t File::write(uint8_t const *buf, size_t size)
{
    return true;
}

int File::read(uint8_t *buf, size_t size)
{
    return true;
}

bool File::seek(uint32_t pos)
{
    return true;
}

File::operator bool (void)
{
    return true;
}

File fs;













#endif
