#pragma once
#include <math.h>
#include "board_pins.h"

/* Vierleitermessung (Kelvin), hergeleitet aus Schaltplan V2_1:
 *
 *   +3V0 --[MUX_pwr]-- A --[R_Kabel]-- B --[MUX_adc]-- ANALOG_R --[R15]-- GND
 *                      |
 *                 [MUX_adc] -- ANALOG_L --[R14]-- GND
 *
 * Gemessen wird differentiell AIN0-AIN1 (= Spannung ueber dem Kabel)
 * plus einseitig AIN1 (= Strom durch R15).
 *
 * Aus KCL am Knoten B folgt exakt:
 *      (u0 - u1) / u1 = R_Kabel / (R_ON + R15)
 * also R_Kabel = (u0-u1)/u1 * R_eff   mit R_eff = R_ON + R15.
 *
 * Die 1k von R15 dominieren R_eff, deshalb schlaegt die Streuung der
 * MUX-Durchlasswiderstaende nur mit ~2% durch: ein einziger
 * Kalibrierwert reicht, keine Tabelle pro Kanal.
 */

/* ponytail: ein globaler Kalibrierwert statt Tabelle pro Kanal.
 * Bei Bedarf gegen Tabelle tauschen, wenn Messungen >2% auseinanderlaufen. */
extern float g_R_eff;   /* Startwert 1070 = 70 (MUX typ.) + 1000 (R15) */

/* u_diff: AIN0-AIN1 in Volt. u_fern: AIN1 gegen GND in Volt. */
inline float kabelwiderstand(float u_diff, float u_fern)
{
    if (u_fern <= 1e-4f) return INFINITY;        /* kein Strom -> offen */
    if (u_diff < 0.0f)   u_diff = 0.0f;          /* Rauschen um 0 */
    return (u_diff / u_fern) * g_R_eff;
}

/* Vorgabe aus Programmstruktur.docx / README */
inline const char* klassifizierung(float r_kabel)
{
    if (isnan(r_kabel))        return "unbekannt";
    if (r_kabel <= 1.0f)       return "Leitend";
    if (r_kabel <= 1000.0f)    return "Schluss";
    if (r_kabel <= 100000.0f)  return "Hochohmig";
    return "Isolierend";
}

/* Einseitiger Pfad (nur Fernende gemessen, kein Kelvin): der
 * Durchlasswiderstand beider MUX steckt mit im Ergebnis.
 *   u1 = V * R15 / (R_loop0 + R_Kabel)   mit R_loop0 = R_pwr + R_adc + R15
 * Nur fuer Bruecken-/Durchgangserkennung gedacht, nicht fuer 0..1 Ohm:
 * die Aufloesung ist rund 16x schlechter als differentiell. */
extern float g_R_loop0;   /* Startwert 1140 = 70 + 70 + 1000 */

inline float kabelwiderstand_einseitig(float u_fern)
{
    if (u_fern <= 1e-4f) return INFINITY;
    float r = (V_INJECT * R_SHUNT) / u_fern - g_R_loop0;
    return r < 0.0f ? 0.0f : r;
}

/* Einmalige Kalibrierung gegen eine gebrueckte Testbuchse (R_Kabel ~ 0).
 * Liefert R_eff aus einer Messung mit bekanntem Referenzwiderstand. */
inline float kalibriere_R_eff(float u_diff, float u_fern, float r_referenz)
{
    if (u_diff <= 0.0f) return g_R_eff;          /* unbrauchbar, alten Wert behalten */
    return r_referenz * u_fern / u_diff;
}
