#pragma once

#include <Adafruit_LittleFS.h>

#define File Adafruit_LittleFS_Namespace::File 
#define F_WRITE_MODE Adafruit_LittleFS_Namespace::FILE_O_WRITE
#define BUILD_FILE() File(fs)

extern Adafruit_LittleFS fs;

void beginPhy();
bool cleanPhy();
