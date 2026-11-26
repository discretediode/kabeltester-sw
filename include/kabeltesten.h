#pragma once
#include <stdint.h>

/* Topologie laut Schaltplan V2_1:
 *   Linker Stecker  = 32 Pole, Sockets L1 (Pol 0..15) + L2 (Pol 16..31)
 *   Rechter Stecker = 32 Pole, Sockets R1 (Pol 0..15) + R2 (Pol 16..31)
 * Je Pol existieren zwei MUX-Pfade: Einspeisung (PWR) und Messung (ADC).
 */
enum Seite : uint8_t { LINKS = 0, RECHTS = 1 };

struct Messwert {
    float       r_ohm;
    const char* klasse;
    bool        kelvin;    /* true = differentiell gemessen (praezise)   */
    bool        gueltig;   /* false = I2C-/ADC-Fehler, Wert unbrauchbar  */
    float       u_diff;    /* Rohwert: Spannung ueber dem Kabel [V]      */
    float       u_fern;    /* Rohwert: Spannung am Fernende [V]          */
};

/* false, wenn Portexpander oder ADC nicht antworten - dann ist jede
 * folgende Messung wertlos. Rueckgabe bitte auswerten. */
bool kabel_ini();

/* Allgemeine Einzelmessung: speist an einem Pol ein, misst an einem anderen.
 * Ist der Messpol auf der Gegenseite UND hat denselben Kanalindex, wird
 * automatisch differentiell (Kelvin) gemessen - sonst einseitig. */
Messwert messe(Seite speise_seite, uint8_t speise_pol,
               Seite mess_seite,   uint8_t mess_pol);

/* Durchgang Pol n links <-> Pol n rechts. Immer Kelvin. */
Messwert messe_durchgang(uint8_t pol);

/* Einmalige Kalibrierung gegen einen bekannten Widerstand an Pol n
 * (beide Seiten gebrueckt). Setzt g_R_eff. true bei Erfolg. */
bool kalibriere(uint8_t pol, float r_referenz);
