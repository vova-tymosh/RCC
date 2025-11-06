#!/bin/bash

# RCC Build Script - Compiles all platforms
# Stops on first failure
set -e

# Copy library to Arduino libraries folder
cd .. && cp -r * ../libraries/RCC && cd - >/dev/null

# LTO flags for size optimization (ESP32 only - nRF has linker issues with LTO)
# Note: Serial compilation warning is harmless and expected
ESP32_LTO='--build-property "compiler.c.extra_flags=-flto" --build-property "compiler.cpp.extra_flags=-flto" --build-property "compiler.c.elf.extra_flags=-flto"'

# ESP32C3 - TestLoco
echo -n "Building ESP32C3...		"
eval arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32C3 --build-path build/esp32c3 $ESP32_LTO TestLoco >/dev/null
echo "✓"

# ESP32C6 - TestLoco
echo -n "Building ESP32C6...		"
eval arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32C6 --build-path build/esp32c6 $ESP32_LTO TestLoco >/dev/null
echo "✓"

# nRF52840 - TestLoco (no LTO)
echo -n "Building nRF-Loco...		"
arduino-cli compile --fqbn Seeeduino:nrf52:xiaonRF52840 --build-path build/nrf52L TestLoco >/dev/null
echo "✓"

# nRF52840 - TestPad (no LTO)
echo -n "Building nRF-Pad...		"
arduino-cli compile --fqbn Seeeduino:nrf52:xiaonRF52840 --build-path build/nrf52P TestPad >/dev/null
echo "✓"
