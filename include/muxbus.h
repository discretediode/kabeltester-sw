#pragma once
#include <Arduino.h>
#include <Wire.h>
#include "board_pins.h"

/* Im Schaltplan teilen sich alle vier MUX einer Gruppe EINEN Selectbus
 * (ADC_S0..S3 bzw. PWR_S0..S3); nur die Enables sind einzeln. Deshalb
 * gibt es hier zwei Busobjekte statt acht MUX-Objekten.
 *
 * Folge fuer die Messung: gleichzeitig aktive MUX einer Gruppe zeigen
 * zwangslaeufig auf denselben Kanalindex.
 */
class MuxBus {
public:
    explicit MuxBus(const uint8_t sel[4]) {
        for (uint8_t i = 0; i < 4; i++) _sel[i] = sel[i];
    }
    void ini() {
        for (uint8_t i = 0; i < 4; i++) { pinMode(_sel[i], OUTPUT); digitalWrite(_sel[i], LOW); }
    }
    void setChannel(uint8_t ch) {
        if (ch > 15) return;
        for (uint8_t i = 0; i < 4; i++) digitalWrite(_sel[i], (ch >> i) & 1);
    }
private:
    uint8_t _sel[4];
};

/* PCF8575: traegt die 8 MUX-Enables (aktiv LOW) und die 3 LEDs.
 * Schreiben einer 1 = schwacher Pullup (reicht fuer CMOS-Enable),
 * Schreiben einer 0 = kraeftig LOW. */
class Iox {
public:
    bool begin(uint8_t addr = IOX_I2C_ADDR, TwoWire* wire = &Wire) {
        _addr = addr; _wire = wire;
        _state = 0xFFFF;                 /* alle MUX aus, alle LEDs aus */
        return flush();
    }
    void set(uint8_t bit, bool high) {
        if (high) _state |= (1u << bit); else _state &= ~(1u << bit);
        flush();
    }
    /* Enables einer Gruppe setzen. mask4 Bit 0..3 = L1,L2,R1,R2.
     * Gesetztes Bit = MUX aktiv (Pin wird LOW gezogen). Rest der Gruppe aus.
     * Fuer die Kelvinmessung muessen zwei gleichzeitig aktiv sein (L und R). */
    void enableMask(uint8_t first_bit, uint8_t mask4) {
        for (uint8_t i = 0; i < 4; i++) {
            if (mask4 & (1u << i)) _state &= ~(1u << (first_bit + i));
            else                   _state |=  (1u << (first_bit + i));
        }
        flush();
    }
    bool flush() {
        _wire->beginTransmission(_addr);
        _wire->write((uint8_t)(_state & 0xFF));
        _wire->write((uint8_t)(_state >> 8));
        return _wire->endTransmission() == 0;
    }
private:
    uint8_t   _addr  = IOX_I2C_ADDR;
    TwoWire*  _wire  = &Wire;
    uint16_t  _state = 0xFFFF;
};

/* Globaler Pinindex 0..31 einer Seite -> Socket (0 = L1/R1, 1 = L2/R2) */
inline uint8_t socket_of(uint8_t pin) { return pin / PINS_PRO_SOCKET; }
inline uint8_t channel_of(uint8_t pin) { return pin % PINS_PRO_SOCKET; }
