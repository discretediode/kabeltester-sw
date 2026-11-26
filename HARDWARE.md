# Hardware blockers for the software team

Status: 2026-11-26, checked against `context/Kabeltester Schaltplan V2_1.pdf`
(Rev 002, 2026-11-21). The PCB is not fabricated yet — points 1–3 should be
changed before it goes out.

## 1. BLOCKER: six signals sit on the internal flash bus (GPIO 6–11)

| Net | Header | GPIO |
|------|--------|------|
| `PWR_L2_E`   | SD2  | 9  |
| `ADC_R1_E`   | SD3  | 10 |
| `ADC_R2_E`   | CMD  | 11 |
| `USR_LED_RE` | SD1  | 8  |
| `USR_LED_GE` | SD0  | 7  |
| `USR_LED_GN` | CLK  | 6  |

On the ESP32-WROOM-32E, GPIO 6–11 are hard-wired to the module's SPI flash.
This is not "ugly but workable" — **the module will not boot** with these
driven. Until it is changed, the right-hand measurement board cannot be
addressed at all.

## 2. BLOCKER: the ADS1115 `ADDR` pin (5) is unconnected

With `ADDR` floating the I²C address is undefined and the converter may not
respond at all. **Tie `ADDR` to GND** → address 0x48, which is what the code
already expects (`ADC adc(0x48)`).

## 3. Strapping pins

| Net | GPIO | Risk |
|------|------|--------|
| `PWR_S1` | 0  | LOW at reset = flash download mode, board won't start |
| `PWR_S2` | 2  | must be LOW or floating at reset |
| `PWR_S3` | 15 | LOW suppresses the boot log |
| `ADC_S3` | 12 | MTDI: HIGH at reset sets flash to 1.8 V → boot failure |

GPIO 12 is the dangerous one: a mux select line that happens to sit HIGH at
reset makes the board unusable.

## 4. Root cause: not enough GPIOs

The design needs 8 select lines + 8 enables + 3 LEDs + I²C + SPI + 4 inputs
+ `ADC_RDY` = 30 pins. The module offers 26, four of which are input-only.
That shortfall is why GPIO 6–11 got used.

**Proposal: a PCF8575 (16-bit I²C port expander, ~€1.50) on the I²C bus that
is already there (address 0x20).** It carries all eight mux enables and the
three LEDs. Then only the eight select lines need real GPIOs, and UART
(GPIO 1/3) plus every strapping pin stays free. The software is already
written this way — see `include/board_pins.h` and `include/muxbus.h`.

No I²C address conflicts: ADS1115 0x48, display 0x27, expander 0x20.
Switching latency over I²C is ~0.1 ms at 400 kHz, negligible against the
2 ms settling time.

`PWR_L_E` (IO17) and `PWR_R_E` (IO16) are wired on the mainboard but appear
nowhere on sheets 2 and 3 — two free pins.

## 5. LEDs: reverse the current direction

Right now the LEDs are sourced from the pin through 1 k to GND.
A PCF8575 can only sink (~100 µA source, 25 mA sink).
**Redraw them as +3.3 V → 1 k → expander pin**, otherwise they won't light.

## 6. Not blocking, but must be measured on real hardware

* `SETTLE_MS` in `src/kabeltesten.cpp` is set to 2 ms (estimated from the
  1 k/1 nF input filter). Measure it with a real cable attached.
* `g_R_eff` (default 1070 Ω) and `g_R_loop0` (1140 Ω) need calibrating once
  against a known resistor: `kalibriere(pol, r_referenz)`.
* Mux on-resistance at 3.3 V VCC is more like 100–120 Ω than the datasheet's
  70 Ω (measured at 4.5 V). It folds entirely into the calibration constant,
  so it is not critical.

## 7. What already works regardless

The measurement chain itself is well dimensioned and nobody needs to wait
on it:

* `ANALOG_L` → AIN0 and `ANALOG_R` → AIN1, so `DIFF_0_1` is directly the
  voltage across the cable — a genuine four-wire (Kelvin) measurement.
* R14/R15 (1 k to GND) serve as both the current return and the reference.
* Resolution: 0 Ω versus 1 Ω differ by roughly **296 counts** differentially
  at ±0.256 V. Ample margin.
* `R_cable = (AIN0 − AIN1) / AIN1 × R_eff` is exact, and the 1 k dominates
  `R_eff`, so mux spread only contributes ~2 % error.
* No relays are needed. `relay_driver` can be dropped from the
  Programmstruktur.
