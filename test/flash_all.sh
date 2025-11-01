#!/bin/bash

# RCC Flash Script - Flashes all platforms
# Stops on first failure
set -e

# ESP32C3 - TestLoco (usbmodem1401)
echo -n "Flashing ESP32C3...		"
arduino-cli upload -p /dev/cu.usbmodem1401 --fqbn esp32:esp32:XIAO_ESP32C3 --build-path build/esp32c3 TestLoco >/dev/null
echo "✓"

# ESP32C6 - TestLoco (usbmodem1301)
echo -n "Flashing ESP32C6...		"
arduino-cli upload -p /dev/cu.usbmodem1301 --fqbn esp32:esp32:XIAO_ESP32C6 --build-path build/esp32c6 TestLoco >/dev/null
echo "✓"

# nRF52840 - TestLoco (usbmodem1201)
echo -n "Flashing nRF-Loco...		"
arduino-cli upload -p /dev/cu.usbmodem1201 --fqbn Seeeduino:nrf52:xiaonRF52840 --build-path build/nrf52L TestLoco >/dev/null
echo "✓"

# nRF52840 - TestPad (usbmodem1101)
echo -n "Flashing nRF-Pad...		"
arduino-cli upload -p /dev/cu.usbmodem1101 --fqbn Seeeduino:nrf52:xiaonRF52840 --build-path build/nrf52P TestPad >/dev/null
echo "✓"
