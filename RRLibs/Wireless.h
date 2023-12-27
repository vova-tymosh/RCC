/*
 * Railroad wireless communication
 * 
 *  
 */
#include "Timer.h"

class Wireless {
  private:
    static const int RF_NAME_LENGTH = 6;
    static const int MAX_NAME_LENGTH = 12;
    static const int RF24_PACKET_LEN = 32;
    static const int MAX_FIELD_COUNT = (RF24_PACKET_LEN - 1) / sizeof(float);
    static const int ID_NAMES = 0x80;
    static const int ID_FIELDS = 0x00;

    uint8_t radioName[RF_NAME_LENGTH];
    char fieldNames[MAX_NAME_LENGTH * MAX_FIELD_COUNT];
    float fieldValues[MAX_FIELD_COUNT];
    uint8_t packet[RF24_PACKET_LEN];
    int fieldCount;
    Timer timer;
    int sent;
    int lost;
    int flip;
  public:
    typedef void (* Callback)(char code, float value);
    Wireless(int fieldCount, char* radioName, int cePin, int csnPin);
    void setFieldName(int index, char *name);
    void setFieldValue(int index, float value);
    void setCallback(Callback callback);
    float getLostRate();
    int getSentCount();
    void setup();
    void loop();
  private:
    Callback callback;  
};
