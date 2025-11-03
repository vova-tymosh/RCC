#!/bin/bash

# Parse arguments
BUILD_PATH=""
PORT=""

while [[ $# -gt 0 ]]; do
    case $1 in
        --build-path)
            BUILD_PATH="$2"
            shift 2
            ;;
        --port)
            PORT="$2"
            shift 2
            ;;
    esac
done

if [ -z "$BUILD_PATH" ] || [ -z "$PORT" ]; then
    echo "Error: Missing required arguments"
    echo "Usage: $0 --build-path <path> --port <port>"
    exit 1
fi

# Parse version from Storage.h
VERSION=$(grep "static const uint16_t version" ../src/Storage.h | sed 's/.*= *\([0-9]*\).*/\1/')
if [ -z "$VERSION" ]; then
    echo "Error: Could not parse version from Storage.h"
    exit 1
fi

# Parse partition info from partitions.csv
PARTITION_FILE="$BUILD_PATH/partitions.csv"
if [ ! -f "$PARTITION_FILE" ]; then
    echo "Error: Partition file not found: $PARTITION_FILE"
    exit 1
fi

# Extract spiffs offset and size from partitions.csv
SPIFFS_LINE=$(grep "^spiffs," "$PARTITION_FILE")
if [ -z "$SPIFFS_LINE" ]; then
    echo "Error: Could not find spiffs partition in $PARTITION_FILE"
    exit 1
fi

SPIFFS_OFFSET=$(echo "$SPIFFS_LINE" | cut -d',' -f4 | tr -d ' ')
SPIFFS_SIZE=$(echo "$SPIFFS_LINE" | cut -d',' -f5 | tr -d ' ')

if [ -z "$SPIFFS_OFFSET" ] || [ -z "$SPIFFS_SIZE" ]; then
    echo "Error: Could not parse spiffs offset/size from partition file"
    exit 1
fi

# Convert version to hex bytes (little-endian)
VERSION_HEX=$(printf '\\x%02x\\x%02x' $((VERSION & 0xFF)) $((VERSION >> 8)))

mkdir -p $BUILD_PATH/data
mkdir -p $BUILD_PATH/data/settings
mkdir -p $BUILD_PATH/data/sounds

# Write validation: code (0xC0DE) + version (little-endian)
echo -ne '\xDE\xC0'"$VERSION_HEX" > $BUILD_PATH/data/validation

# Determine which config file to use
CONFIG_FILE="custom.cfg"
if [ ! -f "$CONFIG_FILE" ]; then
    CONFIG_FILE="default.cfg"
    echo "Using default.cfg (custom.cfg not found)"
else
    echo "Using custom.cfg"
fi

# Read config file and create files
while IFS=: read -r key value; do
    # Skip comments and empty lines
    [[ "$key" =~ ^#.*$ ]] && continue
    [[ -z "$key" ]] && continue
    
    # Handle special cases
    if [ "$key" = "functionNames" ]; then
        echo -ne "${value}\x00" > $BUILD_PATH/data/functionNames
    else
        # Regular settings
        echo -ne "${value}\x00" > $BUILD_PATH/data/settings/$key
    fi
done < "$CONFIG_FILE"

# Create filesystem image
~/Library/Arduino15/packages/esp32/tools/mklittlefs/*/mklittlefs \
    -c $BUILD_PATH/data -p 256 -b 4096 -s $SPIFFS_SIZE $BUILD_PATH/littlefs.bin > /dev/null

# Flash to device
~/Library/Arduino15/packages/esp32/tools/esptool_py/*/esptool \
    --port $PORT --baud 921600 write-flash $SPIFFS_OFFSET $BUILD_PATH/littlefs.bin

echo "Done!"