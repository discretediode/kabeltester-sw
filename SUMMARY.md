# Kabeltester — state of the measurement software

Branch: `messung-4-leiter` (branched from `Kabelauslesen`)
Status: 2026-11-26

This document summarises what was built for the measurement chain and why.
Hardware-side blockers are kept separately in [HARDWARE.md](HARDWARE.md).

---

## 1. Starting point

Before these changes there were three diverging branches:

| Branch | Contents |
|---|---|
| `main` | PlatformIO skeleton, two matrices declared in `setup()` |
| `Kabelauslesen` | MUX and ADC libraries, `Kabeltesten.h` with the scan loops |
| `Datenspeicherung` | SD output in `abspeichern.h` |

The MUX library matched the hardware exactly (CD74HC4067, 16 channels,
S0–S3, active-low enable) and was taken over unchanged. The ADS1115 library
was also solidly built and was only extended.

Two substantive problems forced a rework:

* **The topology was wrong.** `Kabeltesten.h` treated the muxes as poles
  0–15 and 16–31 of *one* connector. Per the schematic there are in fact
  **two connectors of 32 poles each**, every one split across two sockets
  (L1+L2 and R1+R2). So "end-to-end" means: inject on the left, sense on the
  right — not "lower half against upper half".
* **The measurement range was insufficient.** Under the original assumptions
  (5 V, 10 k reference, single-ended against GND) a perfect cable and a 1 Ω
  cable are only 0.5 mV apart, roughly 4 counts. That made the most
  important category, "Leitend" (0–1 Ω), impossible to separate from
  "Schluss".

## 2. The decision: four-wire measurement instead of relays

`Programmstruktur.docx` provided for a `relay_driver` — relays were meant to
take the muxes' ~70 Ω on-resistance out of the measurement path. Schematic
V2_1 contains **no relays**, and none are needed:

The schematic is already laid out for a genuine four-wire measurement. Every
pole has two separate mux paths — "Stromeinspeissung" (PWR) and "ADC Messung"
(ADC). `ANALOG_L` goes to AIN0 and `ANALOG_R` to AIN1, which makes the
ADS1115's differential reading `AIN0 − AIN1` **directly the voltage across
the cable**. Current flows through the muxes while the sense lines stay
essentially current-free — exactly what relays would otherwise be needed for.

**`relay_driver` can be dropped from the Programmstruktur.**

## 3. The measurement

```
  +3V0 --[MUX_pwr]-- A --[R_cable]-- B --[MUX_adc]-- ANALOG_R --[R15]-- GND
                     |
                [MUX_adc] -- ANALOG_L --[R14]-- GND
```

R14 and R15 (1 k each to GND, on the mainboard) act as both the current
return path and the reference resistor. Applying Kirchhoff's current law at
node B gives, exactly:

```
    R_cable = (AIN0 - AIN1) / AIN1 * R_eff        where R_eff = R_MUX + 1k
```

The pleasant part: the 1 k dominates `R_eff`, so the spread in mux
on-resistance only contributes about **2 %** error. A single calibration
constant is enough — no per-channel table.

Resolution at the ±0.256 V range (7.81 µV per count):

| R_cable | AIN0 − AIN1 | Counts |
|---|---|---|
| 0 Ω | 0.002 mV | 0 |
| **1 Ω** | **2.315 mV** | **296** |
| 10 Ω | 22.97 mV | 2940 |
| 1 kΩ | 1.232 V | range too small → autorange |

296 counts between 0 and 1 Ω. Ample margin, so the Leitend/Schluss boundary
resolves safely.

Because the range saturates above roughly 110 Ω, `readVoltAuto()` measures in
two stages: a coarse reading at ±6.144 V, pick the finest range that still
fits, then measure again. Two conversions, up to 16× better resolution.

### The single-ended path

All four ADC muxes share **one** select bus (`ADC_S0–S3`). Any two muxes
enabled at the same time therefore necessarily point at the same channel
index. Consequences:

* **Pole n left against pole n right** (continuity): Kelvin possible, precise.
* **Pole i against pole j, i ≠ j** (bridges): single-ended only, roughly 16×
  coarser resolution.

That is sufficient: bridges only need to be *detected*, not measured
precisely. `messe()` selects the mode automatically and reports which one it
used in the `kelvin` field.

## 4. What was built

| File | Contents |
|---|---|
| `include/board_pins.h` | Pin assignment from the schematic, single source of truth |
| `include/muxbus.h` | Shared select bus + PCF8575 for the enables |
| `include/messung.h` | Resistance maths + classification, Arduino-free |
| `include/kabeltesten.h`, `src/kabeltesten.cpp` | Measurement sequence, corrected topology |
| `src/messung.cpp` | The two calibration constants |
| `lib/ADC/src/ADC.*` | Extended with differential mode and autoranging |
| `test/selbsttest.cpp` | Host-side self-test, no hardware required |
| `HARDWARE.md` | Hardware blockers for the software team |

Removed: `include/adc_driver.h` (never implemented, superseded by `lib/ADC`)
and the old `include/Kabeltesten.h` (wrong topology).

### Classification

Per `Programmstruktur.docx` and the README, implemented in `messung.h`:

| Category | Range |
|---|---|
| Leitend | 0 – 1 Ω |
| Schluss | 1 Ω – 1 kΩ |
| Hochohmig | 1 kΩ – 100 kΩ |
| Isolierend | > 100 kΩ |

The differing thresholds in `abspeichern.h` on `Datenspeicherung`
(10 / 100 / 10000, with categories "Widerstand" and "Kritisch") should be
replaced by `klassifizierung()`. That file also contains a classic C trap:
`10 < x < 100` is evaluated as `(10 < x) < 100` and is therefore **always
true** — the "Kritisch" and "Hochohmig" branches are unreachable. Clang
rejects it outright at compile time.

## 5. Building and checking

```sh
# Firmware
pio run                     # -> SUCCESS, RAM 6.7 %, Flash 22.1 %

# Check the measurement maths without hardware
c++ -std=c++17 -I include test/selbsttest.cpp src/messung.cpp -o /tmp/st && /tmp/st
```

The self-test recomputes the reference points from the network analysis, for
both the Kelvin and the single-ended path. Worst case is 0.65 % at 1 Ω
(1000 Ω and 10 Ω land under 0.15 %); that error comes from rounding the
reference voltages to four decimals in the test table, not from the formula —
the derivation itself is exact. It also checks all four category boundaries,
the open-cable case and the calibration function.

`src/main.cpp` is deliberately just one pass over all 32 poles with output to
the serial port — enough to exercise the measurement chain as soon as a board
exists.

## 6. Open items

**To settle before fabrication** (details in [HARDWARE.md](HARDWARE.md)): six
signals sit on GPIO 6–11 (internal flash, the module won't boot), the
ADS1115's `ADDR` pin is unconnected, and four signals sit on strapping pins.
Proposal: a PCF8575 on the I²C bus for the eight mux enables and the three
LEDs — which is what the software already assumes.

**To measure on real hardware:** `SETTLE_MS` (currently 2 ms, estimated), plus
`g_R_eff` and `g_R_loop0` via `kalibriere(pol, r_referenz)`.

**Not built yet, because they belong to other owners:**

| Module | Owner |
|---|---|
| `ui_driver` (display, rotary encoder) | Rafael |
| `storage_driver`, `report_generator` | Luca |
| State machine (WAIT/READ/ADD/SCAN/DISPLAY) | unassigned |
| `config_parser` | unassigned — missing from `Programmstruktur.docx` even though the READ state needs it |

`config_parser` and `report_generator` are still waiting on an **example text
format** for the target configuration. That is not a missing file but a team
decision nobody has made yet.
