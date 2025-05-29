#pragma once

#include <Adafruit_LittleFS.h>

#define File Adafruit_LittleFS_Namespace::File 
#define F_WRITE_MODE Adafruit_LittleFS_Namespace::FILE_O_WRITE
#define BUILD_FILE() File(LittleFS)


class RccFS : public Adafruit_LittleFS
{
public:
    RccFS(struct lfs_config* cfg) : Adafruit_LittleFS(cfg) {}

    bool begin(bool format = true);
};

extern RccFS LittleFS;
