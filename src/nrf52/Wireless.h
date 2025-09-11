/*
 * Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
 * Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
 * For more details go to https://github.com/vova-tymosh/RCC
 *
 * The above copyright notice shall be included in all
 * copies or substantial portions of the Software.
 */
/*
 * Wireless comms (RF24 based)
 *
 * RF Library Instalation
 *   Home page:            https://nrf24.github.io/RF24/index.html
 *   C code Instalation: https://nrf24.github.io/RF24/md_docs_linux_install.html
 *   Python wrapper: https://nrf24.github.io/RF24/md_docs_python_wrapper.html
 *
 *    Need to patch it using this https://github.com/nRF24/RF24/issues/320
 *
 *    Patch for old MacOS:
 *       pip3 install adafruit-nrfutil
 *       cd
 * ~/Library/Arduino15/packages/Seeeduino/hardware/nrf52/1.1.8/tools/adafruit-nrfutil/macos
 *       mv adafruit-nrfutil adafruit-nrfutil.backup
 *       ln -s $(which adafruit-nrfutil)
 *
 *    To debug radio issues try the following:
 *       printf_begin();
 *       radio.printPrettyDetails();
 */

#pragma once

#ifdef ARDUINO_ARCH_NRF52
#include <nrf_to_nrf.h>
#define RADIO_LEVEL NRF_PA_HIGH
#define RADIO_BW NRF_250KBPS
#define RADIO_TYPE nrf_to_nrf
#define RADIO_NETWORK RF52Network
#define RADIO_CTOR radio()

#elif defined(ARDUINO_ARCH_AVR)
#include <RF24.h>
#define RADIO_LEVEL RF24_PA_HIGH
#define RADIO_BW RF24_250KBPS
#define RADIO_TYPE RF24
#define RADIO_NETWORK RF24Network
#define RADIO_CTOR radio(NRF_CE, NRF_CSN)
#else
#error Architecture/Platform is not supported!
#endif
#include <RF24Network.h>

class Wireless
{
protected:
    RADIO_TYPE radio;
    RADIO_NETWORK network;
    static const int RETRY = 20;
    static const uint16_t STATION_NODE = 0;
    uint16_t node;

public:
    Wireless() : RADIO_CTOR, network(radio) {};

    uint16_t read(void *payload, uint16_t size, int *from = NULL)
    {
        RF24NetworkHeader header;
        uint16_t r = network.read(header, payload, size);
        if (from)
            *from = header.from_node;
        return r;
    }

    bool write(const void *payload, uint16_t size, int to = STATION_NODE)
    {
        bool report = false;
        RF24NetworkHeader header(to);
        for (int i = 0; i < RETRY; i++) {
            report = network.write(header, payload, size);
            if (report)
                break;
        }
        return report;
    }

    bool available()
    {
        network.update();
        return network.available();
    }

    void begin(int node)
    {
        this->node = node;
        if (!radio.begin()) {
            Serial.println("ERROR: Radio is dead");
            return;
        }
        radio.setPALevel(RADIO_LEVEL);
        radio.setDataRate(RADIO_BW);
        network.begin(node);
    }
};
