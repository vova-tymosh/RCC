#if defined(ARDUINO_ARCH_NRF52) || defined(ARDUINO_AVR_LEONARDO)

/*
SPI flash storage for nRF52
Important:
    The flash writes with 4 bytes alignment, so the size of the file will be
rounded up to the next multiple of 4 Also FileRecords has to be aligned to 4
bytes


*/

#include <stdint.h>
#include <Arduino.h>
#include "Storage.h"
#include "StoragePhy.h"

struct FileRecord {
    char filename[16];
    uint32_t offset;
    uint32_t size;
};

struct MasterRecord {
    uint32_t validation;
    uint32_t count;
    uint32_t end;
};

MasterRecord mr;

uint32_t createFile(const char *filename, size_t size)
{
    if (mr.end + size > phySize) {
        Serial.println("[FS] Storage full");
        return 0;
    }
    size = (size + 3) & ~3;
    FileRecord f;
    strncpy(f.filename, filename, sizeof(f.filename));
    f.offset = mr.end;
    f.size = size;
    uint32_t recordAt = mr.count * sizeof(FileRecord) + sizeof(MasterRecord);
    phyWrite(recordAt, (const uint8_t *)&f, sizeof(f));
    mr.count++;
    mr.end += size;
    phyWrite(0, (const uint8_t *)&mr, sizeof(mr));
    return f.offset;
}

bool getFile(const char *filename, FileRecord *record)
{
    FileRecord f;
    uint32_t offset = sizeof(mr);
    for (int i = 0; i < mr.count; i++) {
        phyRead(offset + i * sizeof(f), (uint8_t *)&f, sizeof(f));
        if (strcmp(f.filename, filename) == 0) {
            memcpy(record, &f, sizeof(f));
            return true;
        }
    }
    return false;
}

void Storage::beginInternal()
{
    phyBegin();
}

uint32_t Storage::getValidation()
{
    mr.validation = 0;
    phyRead(0, (uint8_t *)&mr, sizeof(mr));
    return mr.validation;
}

void Storage::setValidation(uint32_t validation)
{
    mr = {validation, 0, maxFiles * sizeof(FileRecord)};
    phyWrite(0, (const uint8_t *)&mr, sizeof(mr));
}

void Storage::clearInternal()
{
    phyErase();
}

int Storage::read(const char *filename, void *buffer, size_t size,
                  size_t offset)
{
    FileRecord f;
    if (getFile(filename, &f)) {
        if (offset >= f.size)
            return 0;
        if (offset + size > f.size)
            size -= offset + size - f.size;
        return phyRead(f.offset + offset, (uint8_t *)buffer, size);
    }
    return 0;
}

int Storage::write(const char *filename, void *buffer, size_t size,
                   size_t offset)
{
    FileRecord f;
    if (getFile(filename, &f)) {
        if (offset >= f.size)
            return 0;
        if (offset + size > f.size)
            size -= offset + size - f.size;
        return phyWrite(f.offset + offset, (const uint8_t *)buffer, size);
    } else {
        uint32_t fileOffset = createFile(filename, size);
        if (fileOffset)
            return phyWrite(fileOffset, (const uint8_t *)buffer, size);
    }
    Serial.print("[FS] Failed to write file: ");
    Serial.println(filename);
    return 0;
}

bool Storage::exists(const char *filename)
{
    FileRecord f;
    return getFile(filename, &f);
}

bool Storage::allocate(const char *filename, size_t size)
{
    return (createFile(filename, size) > 0);
}
#endif
