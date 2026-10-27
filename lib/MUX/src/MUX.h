#ifndef MUX_h
#define MUX_h

#include <Arduino.h>

class MUX {
  public:
    // Konstruktor: s0-s3 sind Pflicht, enablePin ist optional (255 = nicht genutzt)
    MUX(uint8_t s0, uint8_t s1, uint8_t s2, uint8_t s3, uint8_t enablePin = 255);

    // Initialisiert die Pins (im setup() aufrufen)
    void begin();

    // Wählt den Kanal (0 bis 15) per Bit-Operation
    void setChannel(uint8_t channel);

    // Aktiviert (LOW) oder deaktiviert (HIGH) den Multiplexer über Pin 15
    void enable();
    void disable();

  private:
    uint8_t _pins[4];
    uint8_t _enablePin;
};

#endif