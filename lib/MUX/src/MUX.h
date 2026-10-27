#ifndef MUX_h
#define MUX_h

#include <Arduino.h>

class MUX {
  public:
    
    MUX(uint8_t s0, uint8_t s1, uint8_t s2, uint8_t s3, uint8_t enablePin );
    
    //SteuerPins definieren und setzt auf Kannal 0
    void ini();

    // Wählt den Kanal
    void setChannel(uint8_t channel);

    // Aktiviert (LOW) oder deaktiviert (HIGH) den Multiplexer über enablePin
    void enable();
    void disable();

  private:
    uint8_t _pins[4];
    uint8_t _enablePin;
};

#endif