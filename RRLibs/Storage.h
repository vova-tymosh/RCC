#include <EEPROM.h>

class Storage {
  const int ADDR_VALIDATION = 0;
  const int ADDR_DATA = 1;

  public:
    uint16_t restore() {
      uint16_t data = 0;
      uint8_t validation = 0;
      EEPROM.get(ADDR_VALIDATION, validation);
      if (validation == 0xFF) {
        validation = 0;
        EEPROM.put(ADDR_VALIDATION, validation);
        EEPROM.put(ADDR_DATA, data);
        Serial.println(F("First time EEPROM init"));
      } else {
        EEPROM.get(ADDR_DATA, data);
      }
      return data;
    }

    void save(uint16_t data) {
      EEPROM.put(ADDR_DATA, data);
    }
};
