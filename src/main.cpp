#include <Arduino.h>
#include "kabeltesten.h"

/* ponytail: nur ein Durchgangs-Scan ueber alle 32 Pole. State Machine,
 * Display und SD-Ausgabe haengen an ui_driver/storage_driver und sind
 * bewusst noch nicht hier verdrahtet. */

void setup()
{
    Serial.begin(115200);
    if (!kabel_ini()) {
        Serial.println("FEHLER: ADS1115 oder Portexpander antwortet nicht");
        return;                       /* loop() meldet weiter unbrauchbare Werte */
    }
    Serial.println("Kabeltester bereit");
}

void loop()
{
    for (uint8_t pol = 0; pol < 32; pol++) {
        Messwert m = messe_durchgang(pol);
        Serial.printf("Pol %2u: %10.2f Ohm  %-11s %s%s\n",
                      pol, m.r_ohm, m.klasse,
                      m.kelvin ? "(4-Leiter)" : "(einseitig)",
                      m.gueltig ? "" : "  [FEHLER]");
    }
    delay(5000);
}
