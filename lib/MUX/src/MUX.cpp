#include "MUX.h"

MUX::MUX(uint8_t s0, uint8_t s1, uint8_t s2, uint8_t s3, uint8_t enablePin) {
  _pins[0] = s0;
  _pins[1] = s1;
  _pins[2] = s2;
  _pins[3] = s3;
  _enablePin = enablePin;
}

void MUX::ini () {
  for (uint8_t i = 0; i < 4; i++) {
    pinMode(_pins[i], OUTPUT);
    digitalWrite(_pins[i], LOW);
  }
  pinMode(_enablePin, OUTPUT);
  disable();
}

void MUX::setChannel(uint8_t channel) {
  if (channel > 15) return; // Ignoriert ungültige Kanalnummern

  for (uint8_t i = 0; i < 4; i++) {
    digitalWrite(_pins[i], (channel >> i) & 0x01);
  }
}

void MUX::enable() {
    digitalWrite(_enablePin, LOW); 
}

void MUX::disable() {
    digitalWrite(_enablePin, HIGH); 
}