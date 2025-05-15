#pragma once

#include <FS.h>
#include <LittleFS.h>

#define fs LittleFS
#define F_WRITE_MODE ((offset) ? FILE_APPEND : FILE_WRITE)

void beginPhy();
