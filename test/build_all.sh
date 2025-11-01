#!/bin/bash

# RCC Build Script - Compiles all platforms
# Stops on first failure
set -e

# Copy library to Arduino libraries folder
cd .. && cp -r * ../libraries/RCC && cd - >/dev/null

# ESP32C3 - TestLoco
echo -n "Building ESP32C3...		"
arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32C3 --build-path build/esp32c3 TestLoco >/dev/null
echo "✓"

# ESP32C6 - TestLoco
echo -n "Building ESP32C6...		"
arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32C6 --build-path build/esp32c6 TestLoco >/dev/null
echo "✓"

# nRF52840 - TestLoco
echo -n "Building nRF-Loco...		"
arduino-cli compile --fqbn Seeeduino:nrf52:xiaonRF52840 --build-path build/nrf52L TestLoco >/dev/null
echo "✓"

# nRF52840 - TestPad
echo -n "Building nRF-Pad...		"
arduino-cli compile --fqbn Seeeduino:nrf52:xiaonRF52840 --build-path build/nrf52P TestPad >/dev/null
echo "✓"
