#if defined(ARDUINO_ARCH_NRF52) 

/*
SPI flash storage for nRF52
*/

#include <Arduino.h>
#include "Storage.h"
#include "Adafruit_LittleFS.h"
#include "Adafruit_SPIFlash.h"

#define LFS_BLOCK_SIZE        4096


const SPIFlash_Device_t XIAO_NRF_FLASH = P25Q16H;
Adafruit_FlashTransport_QSPI flashTransport;
Adafruit_SPIFlash flash(&flashTransport);

static inline uint32_t lba2addr(uint32_t block)
{
  return block * LFS_BLOCK_SIZE;
}

static int _internal_flash_read (const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    uint32_t addr = lba2addr(block) + off;
    if (flash.readBuffer(addr, (uint8_t*)buffer, size))
        return 0;
    return -1;
}

static int _internal_flash_prog (const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
    uint32_t addr = lba2addr(block) + off;
    if (flash.writeBuffer(addr, (const uint8_t*)buffer, size))
        return 0;
    return -1;
}

static int _internal_flash_erase (const struct lfs_config *c, lfs_block_t block)
{
    if (flash.eraseSector(block))
        return 0;
    return -1;
}

static int _internal_flash_sync (const struct lfs_config *c)
{
    flash.syncBlocks();
    return 0;
}

static struct lfs_config extCfg =
{
    .context = NULL,
    .read = _internal_flash_read,
    .prog = _internal_flash_prog,
    .erase = _internal_flash_erase,
    .sync = _internal_flash_sync,
    .read_size = 256,
    .prog_size = 256,
    .block_size = LFS_BLOCK_SIZE,
    .block_count = XIAO_NRF_FLASH.total_size / LFS_BLOCK_SIZE,
    .lookahead = 128,
    .read_buffer = NULL,
    .prog_buffer = NULL,
    .lookahead_buffer = NULL,
    .file_buffer = NULL
};

Adafruit_LittleFS littefs(&extCfg);
Adafruit_LittleFS_Namespace::File file(littefs);


void Storage::beginInternal()
{
    flash.begin(&XIAO_NRF_FLASH, 1);
    if (!littefs.begin()) {
        Serial.println("[FS] Mount failed, erase and format");
        flash.eraseChip();
        flash.waitUntilReady();  
        bool r = littefs.format();
        r = r && littefs.begin();
        if (!r)
            Serial.println("[FS] Format/reinit failed");
    } else {
        Serial.println("[FS] Mount successful");
    }
}

uint32_t Storage::getValidation()
{
    uint32_t validation = 0;
    read("validation", &validation, sizeof(validation));
    return validation;
}

void Storage::setValidation(uint32_t validation)
{
    write("validation", &validation, sizeof(validation));
}

void Storage::clearInternal()
{
    // littefs.rmdir_r("/");
}

int Storage::read(const char *filename, void *buffer, size_t size,
                  size_t offset)
{
    int r = 0;
    String path = String("/") + filename;
    file.open(path.c_str(), Adafruit_LittleFS_Namespace::FILE_O_READ);
    if (file) {
        if (offset >= file.size())
            return 0;
        if (offset + size > file.size())
            size -= offset + size - file.size();
        file.seek(offset);
        r = file.read((uint8_t *)buffer, size);
        file.close();
    }
    return r;
}

int Storage::write(const char *filename, void *buffer, size_t size,
                   size_t offset)
{
    int r = 0;
    String path = String("/") + filename;
    file.open(path.c_str(), Adafruit_LittleFS_Namespace::FILE_O_WRITE);
    if (file) {
        file.seek(offset);
        r = file.write((const uint8_t *)buffer, size);
        file.close();
    } else {
        Serial.print("[FS] Failed to write file: ");
        Serial.println(filename);
    }
    return r;
}

bool Storage::exists(const char *filename)
{
    String path = String("/") + filename;
    return littefs.exists(path.c_str());
}

bool Storage::allocate(const char *filename, size_t size)
{
    return true;
}
#endif
