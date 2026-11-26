#include "kabeltesten.h"
#include "board_pins.h"
#include "muxbus.h"
#include "messung.h"
#include "ADC.h"

/* ponytail: Einschwingzeit als Konstante, nicht als Rechnung. 1k/1nF der
 * Eingangsfilter sind ~1us, der Rest ist Kabelkapazitaet - real nachmessen
 * und hier nachziehen, wenn die ersten Messwerte driften. */
static const uint16_t SETTLE_MS = 2;

static MuxBus adcBus(PIN_ADC_S);
static MuxBus pwrBus(PIN_PWR_S);
static Iox    iox;
static ADC    adc(0x48);

bool kabel_ini()
{
    adcBus.ini();
    pwrBus.ini();
    pinMode(PIN_ADC_RDY, INPUT);

    /* adc.begin() ruft intern Wire.begin() - danach den Takt setzen,
     * sonst faellt der Bus auf die Standardfrequenz zurueck. */
    const bool adc_ok = adc.begin(PIN_I2C_SDA, PIN_I2C_SCL, PIN_ADC_RDY);
    Wire.setClock(400000);
    const bool iox_ok = iox.begin();

    return adc_ok && iox_ok;
}

/* Enablebits je Gruppe: Bit0=L1, Bit1=L2, Bit2=R1, Bit3=R2 */
static inline uint8_t enable_bit(Seite s, uint8_t pol)
{
    return (uint8_t)((s == LINKS ? 0 : 2) + socket_of(pol));
}

Messwert messe(Seite speise_seite, uint8_t speise_pol,
               Seite mess_seite,   uint8_t mess_pol)
{
    Messwert m = { 0.0f, "unbekannt", false, false, 0.0f, 0.0f };
    if (speise_pol >= PINS_PRO_SEITE || mess_pol >= PINS_PRO_SEITE) return m;

    /* Kelvin geht nur, wenn beide ADC-MUX denselben Kanalindex brauchen -
     * die vier ADC-MUX teilen sich den Selectbus ADC_S0..S3. */
    const bool kelvin = (speise_seite != mess_seite) &&
                        (channel_of(speise_pol) == channel_of(mess_pol));

    uint8_t adc_mask = (uint8_t)(1u << enable_bit(mess_seite, mess_pol));
    if (kelvin) adc_mask |= (uint8_t)(1u << enable_bit(speise_seite, speise_pol));

    iox.enableMask(IOX_ADC_L1_E, adc_mask);
    iox.enableMask(IOX_PWR_L1_E, (uint8_t)(1u << enable_bit(speise_seite, speise_pol)));
    adcBus.setChannel(channel_of(mess_pol));
    pwrBus.setChannel(channel_of(speise_pol));
    delay(SETTLE_MS);

    /* ANALOG_L liegt an AIN0, ANALOG_R an AIN1 (Schaltplan Blatt 1). */
    const ADCInput fern = (mess_seite == RECHTS) ? SE_A1 : SE_A0;
    const float u_fern  = adc.readVoltAuto(fern);
    m.u_fern = u_fern;

    if (kelvin) {
        /* DIFF_0_1 = ANALOG_L - ANALOG_R = Spannung ueber dem Kabel.
         * Bei Einspeisung rechts dreht sich das Vorzeichen um. */
        float u_diff = adc.readVoltAuto(DIFF_0_1);
        if (speise_seite == RECHTS) u_diff = -u_diff;
        m.u_diff = u_diff;
        m.r_ohm  = kabelwiderstand(u_diff, u_fern);
    } else {
        m.r_ohm = kabelwiderstand_einseitig(u_fern);
    }

    m.kelvin  = kelvin;
    m.klasse  = klassifizierung(m.r_ohm);
    m.gueltig = (adc.getError() == ADC_OK);

    /* Alle MUX wieder aus - verhindert Querstroeme zwischen zwei Messungen. */
    iox.enableMask(IOX_ADC_L1_E, 0);
    iox.enableMask(IOX_PWR_L1_E, 0);
    return m;
}

Messwert messe_durchgang(uint8_t pol)
{
    return messe(LINKS, pol, RECHTS, pol);
}

bool kalibriere(uint8_t pol, float r_referenz)
{
    const Messwert m = messe_durchgang(pol);
    if (!m.gueltig || !m.kelvin) return false;

    g_R_eff = kalibriere_R_eff(m.u_diff, m.u_fern, r_referenz);
    return true;
}
