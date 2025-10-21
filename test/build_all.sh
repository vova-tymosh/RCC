#!/bin/bash

# RCC Build and Flash Script
# Compiles and flashes TestLoco and TestKeypad to multiple XIAO platforms

echo "=========================================="
echo "RCC Multi-Platform Build and Flash Script"
echo "=========================================="

# Copy library to Arduino libraries folder
echo "Copying RCC library to Arduino libraries..."
cd .. && cp -r * ../libraries/RCC && cd -
echo "✓ Library copied"

echo ""
echo "Building and flashing all platforms..."
echo ""

# ESP32C3 - TestLoco (usbmodem1401)
echo "************* ESP32C3 - TestLoco *************"
echo "Compiling for ESP32C3..."
arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32C3 TestLoco

if [ $? -eq 0 ]; then
    echo "✓ ESP32C3 compilation successful"
    echo "Flashing to ESP32C3 (usbmodem1401)..."
    arduino-cli upload -p /dev/cu.usbmodem1401 --fqbn esp32:esp32:XIAO_ESP32C3 TestLoco
    if [ $? -eq 0 ]; then
        echo "✓ ESP32C3 flash successful"
    else
        echo "✗ ESP32C3 flash failed"
    fi
else
    echo "✗ ESP32C3 compilation failed"
fi
echo ""


# ESP32C6 - TestLoco (usbmodem1301)
echo "************* ESP32C6 - TestLoco *************"
echo "Compiling for ESP32C6..."
arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32C6 TestLoco
if [ $? -eq 0 ]; then
    echo "✓ ESP32C6 compilation successful"
    echo "Flashing to ESP32C6 (usbmodem1301)..."
    arduino-cli upload -p /dev/cu.usbmodem1301 --fqbn esp32:esp32:XIAO_ESP32C6 TestLoco
    if [ $? -eq 0 ]; then
        echo "✓ ESP32C6 flash successful"
    else
        echo "✗ ESP32C6 flash failed"
    fi
else
    echo "✗ ESP32C6 compilation failed"
fi
echo ""

# nRF52840 - TestLoco (usbmodem1201)
echo "************* nRF52840 - TestLoco *************"
echo "Compiling TestLoco for nRF52840..."
arduino-cli compile --fqbn Seeeduino:nrf52:xiaonRF52840 TestLoco
if [ $? -eq 0 ]; then
    echo "✓ nRF52840 TestLoco compilation successful"
    echo "Flashing to nRF52840 (usbmodem1201)..."
    arduino-cli upload -p /dev/cu.usbmodem1201 --fqbn Seeeduino:nrf52:xiaonRF52840 TestLoco
    if [ $? -eq 0 ]; then
        echo "✓ nRF52840 TestLoco flash successful"
    else
        echo "✗ nRF52840 TestLoco flash failed"
    fi
else
    echo "✗ nRF52840 TestLoco compilation failed"
fi
echo ""

# nRF52840 - TestKeypad (usbmodem1101)
echo "************* nRF52840 - TestKeypad *************"
echo "Compiling TestKeypad for nRF52840..."
arduino-cli compile --fqbn Seeeduino:nrf52:xiaonRF52840 TestKeypad
if [ $? -eq 0 ]; then
    echo "✓ nRF52840 TestKeypad compilation successful"
    echo "Flashing to nRF52840 (usbmodem1101)..."
    arduino-cli upload -p /dev/cu.usbmodem1101 --fqbn Seeeduino:nrf52:xiaonRF52840 TestKeypad
    if [ $? -eq 0 ]; then
        echo "✓ nRF52840 TestKeypad flash successful"
    else
        echo "✗ nRF52840 TestKeypad flash failed"
    fi
else
    echo "✗ nRF52840 TestKeypad compilation failed"
fi

echo ""
echo "=========================================="
echo "Build and flash process completed!"
echo "=========================================="
echo ""
echo "Device Summary:"
echo "• ESP32C3 (1401)  - TestLoco"
echo "• ESP32C6 (1301)  - TestLoco"
echo "• nRF52840 (1201) - TestLoco"
echo "• nRF52840 (1101) - TestKeypad"