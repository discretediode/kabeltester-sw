
#include <Arduino.h>
#include "ADC.h"

// Pins am ESP32 Dev Module definieren
#define PIN_SDA  21
#define PIN_SCL  22
#define PIN_ALRT -1 // Auf Pin-Nummer ändern (z.B. 4), falls ALRT genutzt wird

ADC adc(0x48);

void setup() {
    Serial.begin(115200);
    delay(1000);

    if (!adc.begin(PIN_SDA, PIN_SCL, PIN_ALRT)) {
        Serial.println("Fehler: KY-053 (ADS1115) nicht erreichbar!");
        while (true) { delay(100); }
    }

    Serial.println("KY-053 erfolgreich gestartet!");
}

void loop() {
    // 1. Rohwert an A0 messen und über die Funktion in Volt umrechnen
    int16_t rawA0 = adc.readRaw(CH_A0);
    float voltA0  = adc.rawToVolt(rawA0);

    // 2. Oder direkt in Volt messen (für A1 bis A3)
    float voltA1  = adc.readVolt(CH_A1);

    // Prüfen, ob die I2C-Übertragung fehlerfrei war
    if (adc.getError() == ADC_OK) {
        Serial.printf("A0: %6d Raw -> %5.3f V | A1: %5.3f V\n", rawA0, voltA0, voltA1);
    } else {
        Serial.println("Lesefehler auf dem I2C-Bus!");
    }

    delay(500);
}