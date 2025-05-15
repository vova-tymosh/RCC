#pragma once

#include <Adafruit_LittleFS.h>

#define File Adafruit_LittleFS_Namespace::File 
#define F_WRITE_MODE Adafruit_LittleFS_Namespace::FILE_O_WRITE

extern Adafruit_LittleFS fs;

void beginPhy();
