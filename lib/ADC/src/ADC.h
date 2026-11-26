
#pragma once
#include <Arduino.h>
#include <Wire.h>

// Fehlercodes (übernommen aus ADS1X15.h)
#define ADC_OK                0
#define ADC_ERROR_TIMEOUT    -101
#define ADC_ERROR_I2C        -102

// Eingangskonfiguration des ADS1115 (MUX-Bits, bereits auf Position 12..14)
// Differentiell: misst die Differenz zweier Pins, nicht gegen GND.
// DIFF_0_1 ist beim Kabeltester die Spannung ueber dem Kabel (ANALOG_L - ANALOG_R).
enum ADCInput : uint16_t {
    DIFF_0_1 = 0x0000,   // AIN0 - AIN1
    DIFF_0_3 = 0x1000,
    DIFF_1_3 = 0x2000,
    DIFF_2_3 = 0x3000,
    SE_A0    = 0x4000,   // AIN0 gegen GND
    SE_A1    = 0x5000,
    SE_A2    = 0x6000,
    SE_A3    = 0x7000
};

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

    // Wie readRaw, aber mit freier Eingangswahl - auch differentiell.
    int16_t readRawInput(ADCInput input);
    float   readVoltInput(ADCInput input);

    // Misst zuerst grob (+-4.096V, uebersteuert nie), waehlt daraus den
    // feinsten noch passenden Bereich und misst erneut: 2 Wandlungen,
    // bis zu 16x bessere Aufloesung. Stellt den alten Gain wieder her.
    float   readVoltAuto(ADCInput input);

    // true, wenn die letzte Wandlung am Bereichsende klebte
    bool    wasSaturated() const { return _saturated; }

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
    bool     _saturated = false;

    float    _getMaxVoltage();
    bool     _writeRegister(uint8_t reg, uint16_t value);
    uint16_t _readRegister(uint8_t reg);
};