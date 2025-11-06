#!/bin/bash

# RCC FS Script - Makes and uploads file system to all platforms
# Stops on first failure
set -e

echo -n "Updating FS ESP32C3...		"
./make_fs.sh --build-path build/esp32c3 --port /dev/cu.usbmodem1401 --addr 4 >/dev/null
echo "✓"

echo -n "Updating FS ESP32C6...		"
./make_fs.sh --build-path build/esp32c6 --port /dev/cu.usbmodem1301 --addr 3 >/dev/null
echo "✓"
