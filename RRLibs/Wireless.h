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
*/
#pragma once
#include <RF24.h>
#include <RF24Network.h>


class Wireless {
  private:
    static const uint16_t STATION_NODE = 0;
    int sent;
    int lost;
    RF24 radio;
    RF24Network network;
    uint16_t node;

  public:
    typedef void (* Callback)(char code, float value);
    Wireless(int node, int cePin, int csnPin) : 
      node(node), radio(cePin, csnPin), network(radio) {};

    float getLostRate() {
      if (sent)
        return (float)lost / (float)sent;
      else
        return 0;
    }

    uint16_t read(void* payload, uint16_t size) {
      RF24NetworkHeader header;
      return network.read(header, payload, size);    
    }
    
    bool write(const void* payload, uint16_t size) {
      RF24NetworkHeader header(STATION_NODE);
      bool report = network.write(header, payload, size);
      if (report)
        sent++;
      else
        lost++;
      return report;
    }
    
    void setup() {
      if (!radio.begin()) {
        Serial.println("ERROR: Radio is dead");
        while (1) {}
      }
      radio.setPALevel(RF24_PA_HIGH);
      radio.setDataRate(RF24_250KBPS);
      network.begin(node);
//      printf_begin();
//      radio.printPrettyDetails();
    }
    
    bool available() {
      network.update();
      return network.available();
    }
};
