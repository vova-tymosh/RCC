cd ..
cp -r * ../libraries/RCC
cd test

cd TestLoco
echo "************* Build Loco for ESP32-C3 *************"
arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32C3 && arduino-cli upload -p /dev/cu.usbmodem1301 --fqbn esp32:esp32:XIAO_ESP32C3

# echo "************* Build Loco for NRF52 *************"
# arduino-cli compile --fqbn Seeeduino:nrf52:xiaonRF52840 && arduino-cli upload -p /dev/cu.usbmodem1401 --fqbn Seeeduino:nrf52:xiaonRF52840
cd ..

# cd TestKeypad
# echo "************* Build Pad for NRD52 *************"
# arduino-cli compile --fqbn Seeeduino:nrf52:xiaonRF52840" && arduino-cli upload -p /dev/cu.usbmodem1101 --fqbn Seeeduino:nrf52:xiaonRF52840
# cd ..