
#include "Mux.h"
#include <Arduino.h>

MUX::MUX(uint8_t s0, uint8_t s1, uint8_t s2, uint8_t s3, uint8_t sigPin, uint8_t enablePin) {
  _pins[0] = s0;
  _pins[1] = s1;
  _pins[2] = s2;
  _pins[3] = s3;
  _sigPin = sigPin;
  _enablePin = enablePin;
  _currentChannel = 255; 
}

void MUX::begin() {
  for (uint8_t i = 0; i < 4; i++) {
    if (_pins[i] != 255) {
      pinMode(_pins[i], OUTPUT);
      digitalWrite(_pins[i], LOW);
    }
  }
  
  if (_enablePin != 255) {
    pinMode(_enablePin, OUTPUT);
    enable(); 
  }
}
// Chanel setzten 
void MUX::setChannel(uint8_t channel) {

  if (channel > 15 || channel == _currentChannel) return;

  _currentChannel = channel;

// Pins definieren für Channel
  for (uint8_t i = 0; i < 4; i++) {
    if (_pins[i] != 255) {
      digitalWrite(_pins[i], (channel >> i) & 0x01);
    }
  }
}

void MUX::enable() {
  if (_enablePin != 255) {
    digitalWrite(_enablePin, LOW); // Laut Datenblatt: LOW = Aktiv
  }
}

void MUX::disable() {
  if (_enablePin != 255) {
    digitalWrite(_enablePin, HIGH); // Laut Datenblatt: HIGH = Alle Kanäle getrennt
  }
}

int MUX::readAnalog(uint8_t channel) {
  if (_sigPin == 255) return -1;
  setChannel(channel);
  return analogRead(_sigPin);
}

int MUX::readDigital(uint8_t channel) {
  if (_sigPin == 255) return -1;
  setChannel(channel);
  return digitalRead(_sigPin);
}