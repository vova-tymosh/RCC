#if defined(ARDUINO_ARCH_NRF52)

/*
SPI flash storage for nRF52
*/

#include <Arduino.h>
#include <Adafruit_LittleFS.h>
#include <Adafruit_SPIFlash.h>
#include "../Storage.h"
#include "nrf52/Storage.h"

#define LFS_BLOCK_SIZE 4096

#define P25Q16H                                                                \
    {                                                                          \
        .total_size = (1 << 21), /* 2 MiB */                                   \
        .start_up_time_us = 5000,                                              \
        .manufacturer_id = 0x85,                                               \
        .memory_type = 0x60,                                                   \
        .capacity = 0x15,                                                      \
        .max_clock_speed_mhz = 104,                                            \
        .quad_enable_bit_mask = 0x02,                                          \
        .has_sector_protection = false,                                        \
        .supports_fast_read = true,                                            \
        .supports_qspi = true,                                                 \
        .supports_qspi_writes = true,                                          \
        .write_status_register_split = false,                                  \
        .single_status_byte = false,                                           \
        .is_fram = false,                                                      \
    }

const SPIFlash_Device_t XIAO_NRF_FLASH = P25Q16H;
Adafruit_FlashTransport_QSPI flashTransport;
Adafruit_SPIFlash flash(&flashTransport);

static inline uint32_t lba2addr(uint32_t block)
{
    return block * LFS_BLOCK_SIZE;
}

static int _internal_flash_read(const struct lfs_config *c, lfs_block_t block,
                                lfs_off_t off, void *buffer, lfs_size_t size)
{
    uint32_t addr = lba2addr(block) + off;
    if (flash.readBuffer(addr, (uint8_t *)buffer, size))
        return 0;
    return -1;
}

static int _internal_flash_prog(const struct lfs_config *c, lfs_block_t block,
                                lfs_off_t off, const void *buffer,
                                lfs_size_t size)
{
    uint32_t addr = lba2addr(block) + off;
    if (flash.writeBuffer(addr, (const uint8_t *)buffer, size))
        return 0;
    return -1;
}

static int _internal_flash_erase(const struct lfs_config *c, lfs_block_t block)
{
    if (flash.eraseSector(block))
        return 0;
    return -1;
}

static int _internal_flash_sync(const struct lfs_config *c)
{
    flash.syncBlocks();
    return 0;
}

static struct lfs_config extCfg = {.context = NULL,
                                   .read = _internal_flash_read,
                                   .prog = _internal_flash_prog,
                                   .erase = _internal_flash_erase,
                                   .sync = _internal_flash_sync,
                                   .read_size = 256,
                                   .prog_size = 256,
                                   .block_size = LFS_BLOCK_SIZE,
                                   .block_count = XIAO_NRF_FLASH.total_size /
                                                  LFS_BLOCK_SIZE,
                                   .lookahead = 128,
                                   .read_buffer = NULL,
                                   .prog_buffer = NULL,
                                   .lookahead_buffer = NULL,
                                   .file_buffer = NULL};

RccFS LittleFS(&extCfg);

bool RccFS::begin(bool format)
{
    bool r = true;
    flash.begin(&XIAO_NRF_FLASH, 1);
    if (!Adafruit_LittleFS::begin() && format) {
        r = Adafruit_LittleFS::format();
        r = r && Adafruit_LittleFS::begin();
        if (!r)
            Serial.println("[FS] Format failed");
    }
    return r;
}

void deleteDirectory(const char *dirname)
{
    char path[256];
    File root = LittleFS.open(dirname);
    File file = root.openNextFile();
    while (file) {
        strcpy(path, dirname);
        if (path[strlen(path) - 1] != '/') {
            strcat(path, "/");
        }
        strcat(path, file.name());
        bool isDir = file.isDirectory();
        file = root.openNextFile();
        if (isDir) {
            deleteDirectory(path);
            LittleFS.rmdir(path);
        } else {
            LittleFS.remove(path);
        }
        // Serial.print("[FS] Delete: ");
        // Serial.println(path);
    }
}

void Storage::deleteFiles()
{
    deleteDirectory("/");
    Serial.println("[FS] All deleted");
}

char *Storage::makeSettingsPath(const char *filename, char *buffer, size_t size)
{
    const char *const prefix = "/settings/";
    strcpy(buffer, prefix);
    strncat(buffer, filename, size - strlen(prefix) - 1);
    return buffer;
}

#endif
