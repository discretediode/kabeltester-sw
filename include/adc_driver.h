#ifndef ADC_DRIVER_H
#define ADC_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

/* I2C-Konfiguration laut Schaltplan */
#define I2C_MASTER_NUM         I2C_NUM_0
#define I2C_MASTER_SDA_IO      21         /* GPIO 21 laut ESP32-DevKit Pins */
#define I2C_MASTER_SCL_IO      22         /* GPIO 22 laut ESP32-DevKit Pins */
#define I2C_MASTER_FREQ_HZ     400000     /* 400 kHz Fast-Mode */
#define ADS1115_I2C_ADDR       0x48       /* ADDR-Pin an GND gelegt */

/* ADS1115 Signal-Glättung & Timing */
#define SETTLING_TIME_MS       5          /* Wartezeit für Kondensator-Einstellzeit */
#define ADC_NUM_SAMPLES        8          /* Oversampling zur Messwertstabilisierung */

/**
 * @brief Initialisiert den I2C-Bus und die ADS1115-Register.
 * @return true bei erfolgreicher Initialisierung, sonst false.
 */
bool adc_driver_init(void);

/**
 * @brief Liest den rohen 16-Bit ADC-Wert (Vorzeichenbehaftet) von Kanal AIN0 aus.
 * @return 16-Bit Messwert des ADS1115
 */
int16_t adc_driver_read_raw(void);

/**
 * @brief Wandelt den ausgelesenen Wert in die tatsächliche Spannung in Volt um.
 * @return Spannung an AIN0 in Volt (V)
 */
float adc_driver_read_voltage(void);

#endif /* ADC_DRIVER_H */