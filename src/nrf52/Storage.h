#pragma once

#include <Adafruit_LittleFS.h>

#define File Adafruit_LittleFS_Namespace::File 
#define RCC_FILE_WRITE Adafruit_LittleFS_Namespace::FILE_O_WRITE
#define RCC_FILE_APPEND Adafruit_LittleFS_Namespace::FILE_O_WRITE
#define BUILD_FILE() File(LittleFS)


class RccFS : public Adafruit_LittleFS
{
public:
    RccFS(struct lfs_config* cfg) : Adafruit_LittleFS(cfg) {}

    bool begin(bool format = true);
    
};

extern RccFS LittleFS;
