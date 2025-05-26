#if defined(ARDUINO_ARCH_NRF52) 

/*
SPI flash storage for nRF52
*/

#include <Arduino.h>
#include <Adafruit_LittleFS.h>
#include <Adafruit_SPIFlash.h>
#include "nrf52/Storage.h"


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

#define File Adafruit_LittleFS_Namespace::File 
#define F_WRITE_MODE Adafruit_LittleFS_Namespace::FILE_O_WRITE
#define getPath(x) (String("/") + x.name())

Adafruit_LittleFS fs(&extCfg);

void beginPhy()
{
    flash.begin(&XIAO_NRF_FLASH, 1);
}

bool cleanPhy()
{
    return false;
}

#endif
