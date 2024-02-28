/*
 * Railroad wireless communication
 * 
 *  
 */
#include <RF24.h>
#include <RF24Network.h>
#include "Timer.h"

class Wireless {
  private:
    static const char PACKET_REG = 'r';
    static const char PACKET_NORM = 'n';
    static const int MAX_FIELD_COUNT = 10;
    Timer timer;
    int sent;
    int lost;
    RF24 radio;
    RF24Network network;
    float fieldValues[MAX_FIELD_COUNT];
    uint16_t node;
    int fieldCount;

    uint16_t read(void* payload, uint16_t size);
    bool write(const void* payload, uint16_t size);
    void sendNormal();
  public:
    typedef void (* Callback)(char code, float value);
    Wireless(int node, int fieldCount, int cePin, int csnPin) : 
      node(node), fieldCount(fieldCount), radio(cePin, csnPin), network(radio) {};
    void handshake(char *names);
    void onReceive();
    void setValue(int index, float value);
    void setCallback(Callback callback);
    float getLostRate();
    int getSentCount();
    void setup();
    void loop();
  private:
    Callback callback;  
};
