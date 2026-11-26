#pragma once
#include <stdint.h>

/* Pinbelegung Kabeltester Mainboard, abgeleitet aus
 * "Kabeltester Schaltplan V2_1.pdf" Rev 002 (2026-11-21).
 *
 * ACHTUNG - weicht bewusst vom Schaltplan Rev 002 ab. Dort liegen
 * PWR_L2_E, ADC_R1_E, ADC_R2_E und die drei LEDs auf GPIO 6..11
 * (SD0/SD1/SD2/SD3/CMD/CLK). Das ist der interne SPI-Flash des
 * ESP32-WROOM-32E - damit bootet das Modul nicht. Siehe HARDWARE.md.
 *
 * Fix: alle 8 MUX-Enables + 3 LEDs wandern auf einen PCF8575 am
 * ohnehin vorhandenen I2C-Bus. Dadurch bleiben UART (GPIO1/3) und
 * alle Strapping-Pins (0/2/12/15) frei.
 */

/* --- Selectleitungen: pro Gruppe EIN gemeinsamer Bus fuer alle 4 MUX --- */
/* S0..S3, LSB zuerst (CD74HC4067) */
constexpr uint8_t PIN_ADC_S[4] = {32, 33, 25, 26};   /* "ADC Messung"       */
constexpr uint8_t PIN_PWR_S[4] = {27, 14, 16, 17};   /* "Stromeinspeissung" */

/* --- Direkt am ESP32 --- */
constexpr uint8_t PIN_ADC_RDY = 13;   /* ADS1115 ALERT/RDY */
constexpr uint8_t PIN_I2C_SDA = 21;
constexpr uint8_t PIN_I2C_SCL = 22;
constexpr uint8_t PIN_SPI_CS  = 5;    /* SD-Karte; MOSI 23 / MISO 19 / SCK 18 = VSPI */

/* Nur-Eingang, keine internen Pullups - extern R5..R8 (10k) vorhanden */
constexpr uint8_t PIN_ENC_A   = 36;
constexpr uint8_t PIN_ENC_B   = 39;
constexpr uint8_t PIN_ENC_SW  = 34;
constexpr uint8_t PIN_EXT_SW  = 35;

/* --- Am PCF8575 (I2C 0x20), nicht am ESP32 --- */
constexpr uint8_t IOX_I2C_ADDR = 0x20;
enum IoxBit : uint8_t {
    IOX_ADC_L1_E = 0, IOX_ADC_L2_E, IOX_ADC_R1_E, IOX_ADC_R2_E,
    IOX_PWR_L1_E,     IOX_PWR_L2_E, IOX_PWR_R1_E, IOX_PWR_R2_E,
    IOX_LED_RE = 8,   IOX_LED_GE,   IOX_LED_GN
};

/* --- Messkreis (Schaltplan Blatt 1) --- */
constexpr float V_INJECT  = 3.0f;     /* LP2950-3.0, Netz +3V0        */
constexpr float R_SHUNT   = 1000.0f;  /* R14 / R15 gegen GND          */
constexpr uint8_t PINS_PRO_SOCKET = 16;
constexpr uint8_t PINS_PRO_SEITE  = 32;   /* Socket L1+L2 bzw. R1+R2 */
