/*
 * Railroad wireless communication
 * 
 *  
 */
#include "Timer.h"

class Wireless {
  private:
    static const int RF_NAME_LENGTH = 6;
    static const int MAX_NAME_LENGTH = 10;
    static const int MAX_FIELD_COUNT = 8; //nRF24 don't support more than 32 bytes payload
    uint8_t radioName[RF_NAME_LENGTH];
    char fieldNames[MAX_NAME_LENGTH * MAX_FIELD_COUNT];
    float fieldValues[MAX_FIELD_COUNT];
    int fieldCount;
    Timer timer;
    int sent;
    int lost;
  public:
    typedef void (* Callback)(char code, float value);
    Wireless(int fieldCount, char* radioName, int cePin, int csnPin);
    void setFieldName(int index, char *name);
    void setFieldValue(int index, float value);
    void setCallback(Callback callback);
    float getLostRate();
    void setup();
    void loop();
  private:
    Callback callback;  
};
