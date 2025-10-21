#!/bin/bash

# RCC Build and Flash Script
# Compiles and flashes TestLoco and TestKeypad to multiple XIAO platforms

echo "=========================================="
echo "RCC Multi-Platform Build and Flash Script"
echo "=========================================="

# Copy library to Arduino libraries folder
echo "Copying RCC library to Arduino libraries..."
cp -r .. ../libraries/RCC
echo "✓ Library copied"

echo ""
echo "Building and flashing all platforms..."
echo ""

# ESP32C3 - TestLoco (usbmodem11401)
echo "************* ESP32C3 - TestLoco *************"
echo "Compiling for ESP32C3..."
arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32C3 TestLoco --clean
if [ $? -eq 0 ]; then
    echo "✓ ESP32C3 compilation successful"
    echo "Flashing to ESP32C3 (usbmodem11401)..."
    arduino-cli upload -p /dev/cu.usbmodem11401 --fqbn esp32:esp32:XIAO_ESP32C3 TestLoco
    if [ $? -eq 0 ]; then
        echo "✓ ESP32C3 flash successful"
    else
        echo "✗ ESP32C3 flash failed"
    fi
else
    echo "✗ ESP32C3 compilation failed"
fi
echo ""

# ESP32C6 - TestLoco (usbmodem11301)
echo "************* ESP32C6 - TestLoco *************"
echo "Compiling for ESP32C6..."
arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32C6 TestLoco --clean
if [ $? -eq 0 ]; then
    echo "✓ ESP32C6 compilation successful"
    echo "Flashing to ESP32C6 (usbmodem11301)..."
    arduino-cli upload -p /dev/cu.usbmodem11301 --fqbn esp32:esp32:XIAO_ESP32C6 TestLoco
    if [ $? -eq 0 ]; then
        echo "✓ ESP32C6 flash successful"
    else
        echo "✗ ESP32C6 flash failed"
    fi
else
    echo "✗ ESP32C6 compilation failed"
fi
echo ""

# nRF52840 - TestLoco (usbmodem11201)
echo "************* nRF52840 - TestLoco *************"
echo "Compiling TestLoco for nRF52840..."
arduino-cli compile --fqbn Seeeduino:nrf52:xiaonRF52840 TestLoco
if [ $? -eq 0 ]; then
    echo "✓ nRF52840 TestLoco compilation successful"
    echo "Flashing to nRF52840 (usbmodem11201)..."
    arduino-cli upload -p /dev/cu.usbmodem11201 --fqbn Seeeduino:nrf52:xiaonRF52840 TestLoco
    if [ $? -eq 0 ]; then
        echo "✓ nRF52840 TestLoco flash successful"
    else
        echo "✗ nRF52840 TestLoco flash failed"
    fi
else
    echo "✗ nRF52840 TestLoco compilation failed"
fi
echo ""

# nRF52840 - TestKeypad (usbmodem11101)
echo "************* nRF52840 - TestKeypad *************"
echo "Compiling TestKeypad for nRF52840..."
arduino-cli compile --fqbn Seeeduino:nrf52:xiaonRF52840 TestKeypad
if [ $? -eq 0 ]; then
    echo "✓ nRF52840 TestKeypad compilation successful"
    echo "Flashing to nRF52840 (usbmodem11101)..."
    arduino-cli upload -p /dev/cu.usbmodem11101 --fqbn Seeeduino:nrf52:xiaonRF52840 TestKeypad
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
echo "• ESP32C3 (11401)  - TestLoco"
echo "• ESP32C6 (11301)  - TestLoco" 
echo "• nRF52840 (11201) - TestLoco"
echo "• nRF52840 (11101) - TestKeypad"