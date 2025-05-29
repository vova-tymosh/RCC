#pragma once

#include <FS.h>
#include <LittleFS.h>

#define F_WRITE_MODE ((offset) ? FILE_APPEND : FILE_WRITE)
#define BUILD_FILE() File()

