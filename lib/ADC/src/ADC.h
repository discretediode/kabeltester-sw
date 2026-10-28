
#pragma once
#include <Arduino.h>
#include <Wire.h>

// Fehlercodes (übernommen aus ADS1X15.h)
#define ADC_OK                0
#define ADC_ERROR_TIMEOUT    -101
#define ADC_ERROR_I2C        -102

// Analoge Eingänge A0 bis A3 einfach auswählen
enum ADCChannel : uint8_t {
    CH_A0 = 0,
    CH_A1 = 1,
    CH_A2 = 2,
    CH_A3 = 3
};

// Messbereiche (Gain / Verstärkung)
enum ADCGain : uint16_t {
    GAIN_6_144V = 0x0000, // ±6.144V
    GAIN_4_096V = 0x0200, // ±4.096V (Standard: Ideal für 3.3V am ESP32)
    GAIN_2_048V = 0x0400, // ±2.048V
    GAIN_1_024V = 0x0600, // ±1.024V
    GAIN_0_512V = 0x0800, // ±0.512V
    GAIN_0_256V = 0x0A00  // ±0.256V
};

class ADC {
public:
    // Standardadresse laut Datenblatt ist 0x48 (ADDR an GND)
    ADC(uint8_t address = 0x48, TwoWire *wire = &Wire);

    // Startet I2C mit frei wählbaren Pins (ESP32 Standard: SDA=21, SCL=22) und optionalem ALRT-Pin
    bool begin(int sdaPin = 21, int sclPin = 22, int alrtPin = -1);

    // Prüft, ob der ADC über I2C erreichbar ist
    bool isConnected();

    // Setzt den Messbereich
    void setGain(ADCGain gain);

    // Liest den 16-Bit Rohwert eines Kanals (CH_A0 bis CH_A3)
    int16_t readRaw(ADCChannel channel);

    // Rechnet einen gemessenen Rohwert schnell und präzise in Volt um
    float rawToVolt(int16_t rawValue);

    // Liest den Kanal aus und gibt direkt die Spannung in Volt zurück
    float readVolt(ADCChannel channel);

    // Liest den Status des optionalen ALRT-Pins aus
    bool isAlertActive();

    // Gibt den letzten Fehlercode zurück (0 = alles OK) und setzt ihn zurück
    int8_t getError();

private:
    uint8_t  _address;
    TwoWire* _wire;
    ADCGain  _gain;
    int      _alrtPin;
    int8_t   _error;

    float    _getMaxVoltage();
    bool     _writeRegister(uint8_t reg, uint16_t value);
    uint16_t _readRegister(uint8_t reg);
};