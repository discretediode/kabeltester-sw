/* Hostseitiger Selbsttest der Messmathematik - keine Hardware noetig.
 *   c++ -std=c++17 -I include test/selbsttest.cpp src/messung.cpp -o /tmp/st && /tmp/st
 * Sollwerte stammen aus der Netzwerkanalyse des Schaltplans V2_1
 * (V_INJECT 3.0V, R14=R15=1k, MUX R_ON 70 Ohm).
 */
#include "messung.h"
#include <cassert>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <initializer_list>

static bool nahe(float a, float b, float tol) { return fabsf(a - b) <= tol; }

int main()
{
    /* Stuetzstellen: {u0, u1} in Volt -> erwarteter Kabelwiderstand */
    struct { float u0, u1, r; } f[] = {
        {2.4793f, 2.4793f,      0.0f},
        {2.4795f, 2.4772f,      1.0f},
        {2.4807f, 2.4577f,     10.0f},
        {2.5506f, 1.3184f,   1000.0f},
        {2.6299f, 0.0278f, 100000.0f},
    };

    for (auto& c : f) {
        float r = kabelwiderstand(c.u0 - c.u1, c.u1);
        printf("u0=%.4f u1=%.4f -> R=%10.2f (soll %9.1f)  %s\n",
               c.u0, c.u1, r, c.r, klassifizierung(r));
        assert(nahe(r, c.r, c.r * 0.03f + 0.5f));   /* 3% + 0.5 Ohm */
    }

    /* Klassengrenzen laut Programmstruktur.docx */
    assert(!strcmp(klassifizierung(0.0f),      "Leitend"));
    assert(!strcmp(klassifizierung(0.5f),      "Leitend"));
    assert(!strcmp(klassifizierung(1.0f),      "Leitend"));
    assert(!strcmp(klassifizierung(1.1f),      "Schluss"));
    assert(!strcmp(klassifizierung(1000.0f),   "Schluss"));
    assert(!strcmp(klassifizierung(1001.0f),   "Hochohmig"));
    assert(!strcmp(klassifizierung(100000.0f), "Hochohmig"));
    assert(!strcmp(klassifizierung(100001.0f), "Isolierend"));

    /* Offenes Kabel: kein Strom durch R15 */
    assert(isinf(kabelwiderstand(2.6f, 0.0f)));
    assert(!strcmp(klassifizierung(kabelwiderstand(2.6f, 0.0f)), "Isolierend"));

    /* Kalibrierung: bekannte 10 Ohm muessen R_eff reproduzieren */
    float re = kalibriere_R_eff(2.4807f - 2.4577f, 2.4577f, 10.0f);
    printf("kalibriert R_eff = %.1f Ohm (erwartet ~1070)\n", re);
    assert(nahe(re, 1070.0f, 40.0f));

    /* Einseitiger Pfad: u1 = V*R15/(R_loop0+Rc) */
    for (float rc : {0.0f, 1.0f, 10.0f, 1000.0f}) {
        float u1 = V_INJECT * R_SHUNT / (1140.0f + rc);
        float r  = kabelwiderstand_einseitig(u1);
        printf("einseitig: u1=%.4f -> R=%8.2f (soll %7.1f)\n", u1, r, rc);
        assert(nahe(r, rc, rc * 0.03f + 0.5f));
    }
    assert(isinf(kabelwiderstand_einseitig(0.0f)));

    puts("\nalle Pruefungen bestanden");
    return 0;
}
