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
#include "../Storage.h"
#include "avr/Storage.h"
#include "EEPROM.h"


struct MasterRecord {
    uint16_t count;
    uint16_t end;
};
MasterRecord mr;
File fs;
const int MIN_FILE_SIZE = 16;
const int MAX_FILES = 32;

File LittleFS;

bool File::createRecord(const char *filename, size_t size, FileRecord *record)
{
    if ((mr.end + size > EEPROM.length() || mr.count >= MAX_FILES)) {
        Serial.println("[FS] Storage is full");
        return false;
    }
    strncpy(record->filename, filename, sizeof(record->filename));
    record->addr = mr.end;
    record->size = size;
    uint32_t recordAt = mr.count * sizeof(FileRecord) + sizeof(MasterRecord);
    EEPROM.put(recordAt, *record);
    mr.count++;
    mr.end += size;
    EEPROM.put(0, mr);
    return true;
}

bool File::getRecord(const char *filename, FileRecord *record)
{
    FileRecord r;
    uint32_t recordAt = sizeof(mr);
    for (int i = 0; i < mr.count; i++) {
        EEPROM.get(recordAt + i * sizeof(r), r);
        if (strncmp(r.filename, filename, sizeof(r.filename)) == 0) {
            memcpy(record, &r, sizeof(r));
            return true;
        }
    }
    return false;
}

bool File::begin(bool format)
{
    EEPROM.get(0, mr);
    if (mr.count == 0xFFFF) {
        mr = {0, MAX_FILES * sizeof(FileRecord)};
        EEPROM.put(0, mr);
    }
    return true;
}

File File::openNextFile()
{
    return File(*this);
}

bool File::exists(char const *filename)
{
    FileRecord r;
    return getRecord(filename, &r);
}

File File::open(char const *filename, uint8_t mode = 0)
{
    bool exists = getRecord(filename, &f);
    if (exists)
        isOpen = true;
    else if (mode == 1 && !exists)
        isOpen = createRecord(filename, MIN_FILE_SIZE, &f);
    return File(*this);
}

size_t File::write(uint8_t const *buffer, size_t size)
{
    if (isOpen) {
        if (offset > f.size)
            return 0;
        if (offset + size > f.size)
            size -= offset + size - f.size;
        for (int i = 0; i < size; i++)
            EEPROM.write(f.addr + offset + i, buffer[i]);
        return size;
    }
    return 0;
}

size_t File::read(uint8_t *buffer, size_t size)
{
    if (isOpen) {
        if (offset > f.size)
            return 0;
        if (offset + size > f.size)
            size -= offset + size - f.size;
        for (int i = 0; i < size; i++)
            buffer[i] = EEPROM.read(f.addr + offset + i);
        return size;
    }
    return 0;
}

bool File::seek(uint32_t pos)
{
    if (isOpen)
        offset = pos;
    return isOpen;
}

void Storage::deleteFiles()
{
    mr = {0, MAX_FILES * sizeof(File::FileRecord)};
    EEPROM.put(0, mr);
}

char* Storage::makeSettingsPath(const char *filename, char *buffer, size_t size)
{
    strcpy(buffer, filename);
    return buffer;
}


#endif
