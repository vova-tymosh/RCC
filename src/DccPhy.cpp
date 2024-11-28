#include "DccPhy.h"
#include "DccPipe.h"
#include <Arduino.h>

#define PULSE_SHORT 116
#define PULSE_LONG 200

void dccPhy::setup()
{
    pinMode(9, OUTPUT);
#ifdef ARDUINO_AVR_LEONARDO
    TCCR1A = 0x00;
    TCCR1B = 0x00 | (1 << CS11);
    TIMSK1 = 0x00 | (1 << OCIE1A);

    OCR1A = PULSE_SHORT;
    TCNT1 = 0;
#endif
}

inline void dccPhy::writeHigh()
{
#ifdef ARDUINO_AVR_LEONARDO
    // Clear GPIO D9
    PORTB &= ~0x20;
#endif
}

inline void dccPhy::writeLow()
{
#ifdef ARDUINO_AVR_LEONARDO
    // Set GPIO D9
    PORTB |= 0x20;
#endif
}

void dccPhy::handleInterrupt()
{
#ifdef ARDUINO_AVR_LEONARDO
    static dccPulseState current_state = Preamble;
    static uint8_t pulse = PULSE_SHORT;
    static bool is_second_pulse = false;
    static uint8_t preamble_count = 16;
    static unsigned char outbyte = 0;
    static unsigned char cbit = 0x80;
    static uint8_t byteIndex = 0;
    static Packet cachedMsg;

    if (is_second_pulse) {
        writeLow();
        is_second_pulse = false;
        OCR1A = OCR1A + pulse;
    } else {
        writeHigh();
        is_second_pulse = true;

        switch (current_state) {
        case Preamble:
            pulse = PULSE_SHORT;
            preamble_count--;
            if (preamble_count == 0) {
                current_state = Seperator;
                byteIndex = 0;
                cachedMsg = dcc.getPacket();
            }
            break;
        case Seperator:
            pulse = PULSE_LONG;
            current_state = SendByte;
            cbit = 0x80;
            outbyte = cachedMsg.data[byteIndex];
            break;
        case SendByte:
            if (outbyte & cbit) {
                pulse = PULSE_SHORT;
            } else {
                pulse = PULSE_LONG;
            }
            cbit = cbit >> 1;
            if (cbit == 0) {
                byteIndex++;
                if (byteIndex >= cachedMsg.len) {
                    current_state = Preamble;
                    preamble_count = 16;
                } else {
                    current_state = Seperator;
                }
            }
            break;
        }
        OCR1A = OCR1A + pulse;
    }
#endif
}

#ifdef ARDUINO_AVR_LEONARDO
ISR(TIMER1_COMPA_vect)
{
    dccPhy::handleInterrupt();
}
#endif