
#include "ADC.h"

// Register- und Konfigurations-Konstanten aus ADS1X15.cpp
#define REG_CONVERT         0x00
#define REG_CONFIG          0x01

#define OS_START_SINGLE     0x8000  // Startet eine Einzelmessung
#define OS_NOT_BUSY         0x8000  // Bit ist 1, wenn Messung fertig ist
#define MODE_SINGLE         0x0100  // Stromsparender Einzelmess-Modus
#define DATARATE_128SPS     0x0080  // Standard-Geschwindigkeit (128 Messungen/Sek.)
#define COMP_OFF            0x0003  // Komparator standardmäßig deaktivieren

ADC::ADC(uint8_t address, TwoWire *wire) {
    _address = address;
    _wire    = wire;
    _gain    = GAIN_4_096V; // Ideal für 0 - 3.3V am ESP32
    _alrtPin = -1;
    _error   = ADC_OK;
}

bool ADC::begin(int sdaPin, int sclPin, int alrtPin) {
    // Gültige Adressen laut Datenblatt: 0x48 bis 0x4B
    if ((_address < 0x48) || (_address > 0x4B)) return false;

    _alrtPin = alrtPin;
    if (_alrtPin >= 0) {
        pinMode(_alrtPin, INPUT_PULLUP);
    }

    _wire->begin(sdaPin, sclPin);
    return isConnected();
}

bool ADC::isConnected() {
    _wire->beginTransmission(_address);
    return (_wire->endTransmission() == 0);
}

void ADC::setGain(ADCGain gain) {
    _gain = gain;
}

int16_t ADC::readRaw(ADCChannel channel) {
    if (channel > 3) return 0;

    // Konfiguration zusammenbauen (wie in ADS1X15::_requestADC)
    uint16_t config = OS_START_SINGLE
                    | ((4 + channel) << 12) // Kanal A0..A3 auswählen
                    | _gain
                    | MODE_SINGLE
                    | DATARATE_128SPS
                    | COMP_OFF;

    if (!_writeRegister(REG_CONFIG, config)) {
        return 0;
    }

    // Auf Messergebnis warten mit Timeout (18 ms bei 128 SPS) und yield() für den ESP32
    uint32_t start = millis();
    while ((_readRegister(REG_CONFIG) & OS_NOT_BUSY) == 0) {
        if ((millis() - start) > 18) {
            _error = ADC_ERROR_TIMEOUT;
            return 0;
        }
        yield(); // Verhindert ESP32 Watchdog-Reset
    }

    // 16-Bit Messwert auslesen
    return (int16_t)_readRegister(REG_CONVERT);
}

float ADC::rawToVolt(int16_t rawValue) {
    if (rawValue == 0) return 0.0f;

    // Schnelle Multiplikation statt langsamer Division (aus ADS1X15::toVoltage)
    // 3.0518509475997E-5 entspricht (1.0 / 32767.0)
    return rawValue * _getMaxVoltage() * 3.0518509475997E-5f;
}

float ADC::readVolt(ADCChannel channel) {
    int16_t raw = readRaw(channel);
    return rawToVolt(raw);
}

bool ADC::isAlertActive() {
    if (_alrtPin < 0) return false;
    return digitalRead(_alrtPin) == LOW;
}

int8_t ADC::getError() {
    int8_t err = _error;
    _error = ADC_OK;
    return err;
}

float ADC::_getMaxVoltage() {
    switch (_gain) {
        case GAIN_6_144V: return 6.144f;
        case GAIN_4_096V: return 4.096f;
        case GAIN_2_048V: return 2.048f;
        case GAIN_1_024V: return 1.024f;
        case GAIN_0_512V: return 0.512f;
        case GAIN_0_256V: return 0.256f;
        default:          return 4.096f;
    }
}

bool ADC::_writeRegister(uint8_t reg, uint16_t value) {
    _wire->beginTransmission(_address);
    _wire->write(reg);
    _wire->write((uint8_t)(value >> 8));
    _wire->write((uint8_t)(value & 0xFF));
    if (_wire->endTransmission() != 0) {
        _error = ADC_ERROR_I2C;
        return false;
    }
    return true;
}

uint16_t ADC::_readRegister(uint8_t reg) {
    _wire->beginTransmission(_address);
    _wire->write(reg);
    if (_wire->endTransmission() == 0) {
        if (_wire->requestFrom((int)_address, 2) == 2) {
            uint16_t val = _wire->read() << 8;
            val += _wire->read();
            return val;
        }
    }
    _error = ADC_ERROR_I2C;
    return 0;
}