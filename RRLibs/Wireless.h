/*
 * Wireless comms (RF24 based) 
 * 
 * RF Library Instalation
 *   Home page:            https://nrf24.github.io/RF24/index.html
 *   C code Instalation:   https://nrf24.github.io/RF24/md_docs_linux_install.html
 *   Python wrapper:       https://nrf24.github.io/RF24/md_docs_python_wrapper.html
 *
 *    Need to patch it using this https://github.com/nRF24/RF24/issues/320
 *
 *    Patch for old MacOS:
 *       pip3 install adafruit-nrfutil
 *       cd /Users/XXX/Library/Arduino15/packages/Seeeduino/hardware/nrf52/1.1.8/tools/adafruit-nrfutil/macos
 *       mv adafruit-nrfutil adafruit-nrfutil.backup
 *       ln -s $(which adafruit-nrfutil)
 * 
 *    To debug radio issues try the following:
 *       printf_begin();
 *       radio.printPrettyDetails();
*/

#pragma once

#ifdef RADIO_52
#include <nrf_to_nrf.h>
#define RADIO_LEVEL NRF_PA_HIGH
#define RADIO_BW NRF_250KBPS
#define RADIO_TYPE nrf_to_nrf
#define RADIO_NETWORK RF52Network
#define RADIO_CTOR(x, y) radio()
#else
#include <RF24.h>
#define RADIO_LEVEL RF24_PA_HIGH
#define RADIO_BW RF24_250KBPS
#define RADIO_TYPE RF24
#define RADIO_NETWORK RF24Network
#define RADIO_CTOR(x, y) radio(x, y)
#endif
#include <RF24Network.h>

class WirelessBase {
  protected:
    static const uint16_t STATION_NODE = 0;
    uint16_t node;
    int total;
    int lost;

  public:
    WirelessBase(int node) : node(node) {};

    uint16_t getLostRate() {
      uint16_t lostRate = 0;
      if (total) {
        lostRate = 100 * lost / total;
        if (lostRate > 100)
          lostRate = 100;
      }
      return lostRate;
    }

    virtual uint16_t read(void* payload, uint16_t size) = 0;
    virtual bool write(const void* payload, uint16_t size) = 0;
    virtual bool available() = 0;
    virtual void setup() = 0;
};

class Wireless: public WirelessBase {
  protected:
    RADIO_TYPE radio;
    RADIO_NETWORK network;

  public:
    Wireless(int node, int cePin = 0, int csnPin = 0) :
      WirelessBase(node), RADIO_CTOR(cePin, csnPin), network(radio) {};

    uint16_t read(void* payload, uint16_t size) {
      RF24NetworkHeader header;
      return network.read(header, payload, size);
    }

    bool write(const void* payload, uint16_t size) {
      total++;
      RF24NetworkHeader header(STATION_NODE);
      bool report = network.write(header, payload, size);
      if (!report)
        lost++;
      return report;
    }

    bool available() {
      network.update();
      return network.available();
    }

    void setup() {
      if (!radio.begin()) {
        Serial.println("ERROR: Radio is dead");
        while (1) {}
      }
      radio.setPALevel(RADIO_LEVEL);
      radio.setDataRate(RADIO_BW);
      network.begin(node);
    }
};
