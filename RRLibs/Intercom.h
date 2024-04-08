/*
 * 
 * 
 * 
 * 
 */
#pragma once


class Intercom {
  public:
    typedef float data_t;  
  private:
    static const int I2C_CODE = 1;
    static const int MAX_FIELD = 2;
    static const int PACKET_LEN = MAX_FIELD * sizeof(data_t) ;
    uint8_t size;
    bool master;
    data_t packetIn[MAX_FIELD];
    data_t packetOut[MAX_FIELD];
    int internalRecv();

  public:
    void onReceive(int bytes);
    void onSend();

    void setup(bool master);
    void send(data_t* data, uint8_t size);
    void recv(data_t* data, uint8_t size);
};
extern Intercom intercom;

 
