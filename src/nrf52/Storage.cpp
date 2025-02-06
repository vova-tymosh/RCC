#if defined(ARDUINO_ARCH_NRF52)

/*
SPI flash storage for nRF52
Important:
    The flash writes with 4 bytes alignment, so the size of the file will be rounded up to the next multiple of 4
    Also FileRecords has to be aligned to 4 bytes


*/

#include <stdint.h>
#include "Adafruit_SPIFlash.h"
#include "Storage.h"


struct FileRecord {
    char filename[8];
    uint32_t offset;
    uint32_t size;
};

struct MasterRecord {
    uint32_t validation;
    uint32_t count;
    uint32_t end;
};

const SPIFlash_Device_t XIAO_NRF_FLASH = P25Q16H;
Adafruit_FlashTransport_QSPI flashTransport;
Adafruit_SPIFlash flash(&flashTransport);
MasterRecord mr;

const uint16_t maxFiles = 64;
const int storageSize = 2 * 1024 * 1024;


uint32_t createFile(const char *filename, size_t size)
{
    if (mr.end + size > storageSize) {
        Serial.println("[FS] Storage full");
        return 0;
    }
    size = (size + 3) & ~3;
    FileRecord f;
    strncpy(f.filename, filename, sizeof(f.filename));
    f.offset = mr.end;
    f.size = size;
    uint32_t recordAt = mr.count * sizeof(FileRecord) + sizeof(MasterRecord);
    flash.writeBuffer(recordAt, (const uint8_t*)&f, sizeof(f));
    mr.count++;
    mr.end += size;
    flash.writeBuffer(0, (const uint8_t*)&mr, sizeof(mr));
    return f.offset;
}

bool getFile(const char *filename, FileRecord *record)
{
    FileRecord f;
    uint32_t offset = sizeof(mr);
    for(int i = 0; i < mr.count; i++) {
        flash.readBuffer(offset + i * sizeof(f), (uint8_t*)&f, sizeof(f));
        if (strcmp(f.filename, filename) == 0) {
            memcpy(record, &f, sizeof(f));
            return true;
        }
    }
    return false;
}




void Storage::beginInternal()
{
    flash.begin(&XIAO_NRF_FLASH, 1);
}

uint32_t Storage::getValidation()
{
    mr.validation = 0;
    flash.readBuffer(0, (uint8_t*)&mr, sizeof(mr));
    return mr.validation;
}

void Storage::setValidation(uint32_t validation)
{
    mr = {validation, 0, maxFiles * sizeof(FileRecord)};
    flash.writeBuffer(0, (const uint8_t*)&mr, sizeof(mr));
}

void Storage::clearInternal()
{
    flash.eraseChip();
    flash.waitUntilReady();
}

uint Storage::read(const char *filename, void *buffer, size_t size, uint offset)
{
    FileRecord f;
    if (getFile(filename, &f)) {
        if (offset >= f.size)
            return 0;
        if (offset + size > f.size)
            size -= offset + size - f.size;
        return flash.readBuffer(f.offset + offset, (uint8_t*)buffer, size);
    }
    return 0;
}

uint Storage::write(const char *filename, void *buffer, size_t size, uint offset)
{
    FileRecord f;
    if (getFile(filename, &f)) {
        if (offset >= f.size)
            return 0;
        if (offset + size > f.size)
            size -= offset + size - f.size;
        return flash.writeBuffer(f.offset + offset, (const uint8_t*)buffer, size);
    } else {
        uint32_t fileOffset = createFile(filename, size);
        if (fileOffset)
            return flash.writeBuffer(fileOffset, (const uint8_t*)buffer, size);
    }
    Serial.print("[FS] Failed to write file: ");
    Serial.println(filename);
    return 0;
}

bool Storage::allocate(const char *filename, size_t size)
{
    return (createFile(filename, size) > 0);
}
#endif
